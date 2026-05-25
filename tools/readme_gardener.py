#!/usr/bin/env python3
"""Keep README implementation status summaries in sync.

The detailed status tables in README.md are the source of truth.  This
script derives the top-level README summary and docs/status.* dashboards
from those tables so implementation status changes do not need to be
counted by hand.
"""

from __future__ import annotations

import argparse
import dataclasses
import html
import json
import re
import sys
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


STANDARDS = (17, 20, 23, 26)
STATUS_START = "<!-- preview-status-table:start -->"
STATUS_END = "<!-- preview-status-table:end -->"
DETAIL_HEADING = "## Detailed status"
SUMMARY_HEADING = "## Implementation Status"

HEADER_RE = re.compile(r"^#### `<([^>]+)>`\s*$", re.MULTILINE)
STATUS_BADGE_RE = re.compile(r"!\[\]\[c(?P<std>17|20|23|26)(?P<state>ok|no)(?:\d+)?\]")


@dataclasses.dataclass
class Count:
    implemented: int = 0
    total: int = 0

    def add(self, implemented: bool) -> None:
        self.total += 1
        if implemented:
            self.implemented += 1

    def extend(self, other: "Count") -> None:
        self.implemented += other.implemented
        self.total += other.total

    @property
    def percent(self) -> Optional[int]:
        if self.total == 0:
            return None
        return int((self.implemented * 100.0 / self.total) + 0.5)

    @property
    def percent_float(self) -> float:
        if self.total == 0:
            return 0.0
        return self.implemented * 100.0 / self.total

    def as_dict(self) -> Dict[str, object]:
        return {
            "implemented": self.implemented,
            "total": self.total,
            "percent": self.percent,
        }


@dataclasses.dataclass
class HeaderStatus:
    name: str
    unavailable: bool = False
    total: Count = dataclasses.field(default_factory=Count)
    standards: Dict[int, Count] = dataclasses.field(
        default_factory=lambda: {std: Count() for std in STANDARDS}
    )

    @property
    def has_tracked_status(self) -> bool:
        return self.total.total > 0

    @property
    def has_implemented_status(self) -> bool:
        return self.total.implemented > 0

    def as_dict(self) -> Dict[str, object]:
        return {
            "name": self.name,
            "unavailable": self.unavailable,
            "total": self.total.as_dict(),
            "standards": {
                f"C++{std}": self.standards[std].as_dict() for std in STANDARDS
            },
        }


@dataclasses.dataclass
class StatusModel:
    headers: List[HeaderStatus]
    overall: HeaderStatus


def table_cells(line: str) -> List[str]:
    return [cell.strip() for cell in line.strip().strip("|").split("|")]


def is_table_separator(cells: Sequence[str]) -> bool:
    return all(cell and set(cell) <= {"-", ":", " "} for cell in cells)


def section_is_unavailable(section: str) -> bool:
    for line in section.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if stripped == "N/A":
            return True
        if stripped.startswith("|"):
            return False
        if stripped.startswith("*"):
            return False
    return False


def iter_feature_rows(section: str) -> Iterable[List[str]]:
    for line in section.splitlines():
        if not line.lstrip().startswith("|"):
            continue

        cells = table_cells(line)
        if len(cells) < 3:
            continue
        if is_table_separator(cells):
            continue
        if not cells[0] or cells[0].lower() == "introduced":
            continue
        if not ("`" in cells[0] or STATUS_BADGE_RE.search(line)):
            continue
        if not any(STATUS_BADGE_RE.search(cell) for cell in cells[1:]):
            continue

        yield cells


def parse_readme_status(readme_text: str) -> StatusModel:
    detail_start = readme_text.find(DETAIL_HEADING)
    if detail_start < 0:
        raise ValueError(f"Could not find {DETAIL_HEADING!r} in README")

    detail_text = readme_text[detail_start:]
    matches = list(HEADER_RE.finditer(detail_text))
    if not matches:
        raise ValueError("Could not find any detailed header status sections")

    headers: List[HeaderStatus] = []
    for index, match in enumerate(matches):
        name = match.group(1)
        end = matches[index + 1].start() if index + 1 < len(matches) else len(detail_text)
        section = detail_text[match.end() : end]
        status = HeaderStatus(name=name, unavailable=section_is_unavailable(section))

        if not status.unavailable:
            for cells in iter_feature_rows(section):
                badges = []
                for cell in cells[1:]:
                    badges.extend(STATUS_BADGE_RE.finditer(cell))

                if badges:
                    status.total.add(any(match.group("state") == "ok" for match in badges))

                for badge in badges:
                    std = int(badge.group("std"))
                    status.standards[std].add(badge.group("state") == "ok")

        headers.append(status)

    overall = HeaderStatus(name="Total")
    for header in headers:
        if header.unavailable:
            continue
        overall.total.extend(header.total)
        for std in STANDARDS:
            overall.standards[std].extend(header.standards[std])

    return StatusModel(headers=headers, overall=overall)


def badge(count: Count) -> str:
    if count.total == 0:
        return ""
    percent = count.percent
    assert percent is not None
    return (
        f"![](https://img.shields.io/badge/{count.implemented}/{count.total}-grey)"
        f"![][p{percent:03d}]"
    )


def count_text(count: Count) -> str:
    if count.total == 0:
        return ""
    percent = count.percent
    assert percent is not None
    return f"{count.implemented}/{count.total} ({percent}%)"


def header_link(name: str) -> str:
    return f"[{name}](#{name.lower()})"


def markdown_table(headers: Sequence[str], rows: Sequence[Sequence[str]]) -> str:
    widths = [len(header) for header in headers]
    for row in rows:
        for index, cell in enumerate(row):
            widths[index] = max(widths[index], len(cell))

    def format_row(row: Sequence[str]) -> str:
        padded = [cell.ljust(widths[index]) for index, cell in enumerate(row)]
        return "| " + " | ".join(padded) + " |"

    separator = "|-" + "-|-".join("-" * width for width in widths) + "-|"
    lines = [format_row(headers), separator]
    lines.extend(format_row(row) for row in rows)
    return "\n".join(lines)


def summary_row(status: HeaderStatus) -> List[str]:
    if status.name == "Total":
        name = ""
        total = badge(status.total)
    else:
        name = header_link(status.name)
        total = "N/A" if status.unavailable else badge(status.total)

    return [
        name,
        total,
        "",
        badge(status.standards[17]),
        badge(status.standards[20]),
        badge(status.standards[23]),
        badge(status.standards[26]),
    ]


def build_readme_status_block(model: StatusModel) -> str:
    active = [
        header
        for header in model.headers
        if not header.unavailable and header.has_implemented_status
    ]
    backlog = [header for header in model.headers if header not in active]

    rows: List[List[str]] = [summary_row(model.overall), ["", "", "", "", "", "", ""]]
    rows.extend(summary_row(header) for header in active)
    rows.append(["", "", "", "", "", "", ""])
    rows.extend(summary_row(header) for header in backlog)

    table = markdown_table(
        ["Header", "Total", "", "C++17", "C++20", "C++23", "C++26"],
        rows,
    )

    return "\n".join(
        [
            STATUS_START,
            "Implementation available in C++14 ~ :",
            "",
            "Generated from [Detailed status](#detailed-status). Run "
            "`python3 tools/readme_gardener.py --write` after editing detailed feature rows.",
            "Dashboards: [implementation](docs/status.md), [coverage](docs/coverage.md).",
            "",
            table,
            STATUS_END,
        ]
    )


def replace_readme_status_block(readme_text: str, block: str) -> str:
    marker_re = re.compile(
        re.escape(STATUS_START) + r".*?" + re.escape(STATUS_END),
        re.DOTALL,
    )
    if marker_re.search(readme_text):
        return marker_re.sub(block, readme_text)

    heading = readme_text.find(SUMMARY_HEADING)
    if heading < 0:
        raise ValueError(f"Could not find {SUMMARY_HEADING!r} in README")

    after_heading = readme_text.find("\n", heading)
    if after_heading < 0:
        raise ValueError("README ends immediately after implementation heading")
    after_heading += 1

    separator = re.search(r"(?m)^-----\s*$", readme_text[after_heading:])
    if not separator:
        raise ValueError("Could not find separator after implementation summary")

    separator_start = after_heading + separator.start()
    return readme_text[:after_heading] + block + "\n\n" + readme_text[separator_start:]


def build_dashboard_markdown(model: StatusModel) -> str:
    standard_rows = [
        [f"C++{std}", count_text(model.overall.standards[std])]
        for std in STANDARDS
    ]

    header_rows = []
    for header in model.headers:
        if header.unavailable:
            total = "N/A"
        else:
            total = count_text(header.total)
        header_rows.append(
            [
                f"[{header.name}](../README.md#{header.name.lower()})",
                total,
                count_text(header.standards[17]),
                count_text(header.standards[20]),
                count_text(header.standards[23]),
                count_text(header.standards[26]),
            ]
        )

    backlog_rows = []
    for header in model.headers:
        if header.unavailable:
            backlog_rows.append([header.name, "N/A"])
        elif header.total.total and header.total.implemented < header.total.total:
            missing = header.total.total - header.total.implemented
            backlog_rows.append([header.name, str(missing)])
        elif header.total.total == 0:
            backlog_rows.append([header.name, "No tracked entries"])

    lines = [
        "# STL Preview Status Dashboard",
        "",
        "Generated from [README.md](../README.md#detailed-status) by "
        "[tools/readme_gardener.py](../tools/readme_gardener.py).",
        "",
        "## Overall",
        "",
        markdown_table(
            ["Scope", "Implementation"],
            [["Total", count_text(model.overall.total)]] + standard_rows,
        ),
        "",
        "## Headers",
        "",
        markdown_table(
            ["Header", "Total", "C++17", "C++20", "C++23", "C++26"],
            header_rows,
        ),
        "",
        "## Review Queue",
        "",
        markdown_table(["Header", "Remaining tracked entries"], backlog_rows),
        "",
    ]
    return "\n".join(lines)


def build_dashboard_json(model: StatusModel) -> str:
    payload = {
        "generated_by": "tools/readme_gardener.py",
        "source": "README.md#detailed-status",
        "standards": [f"C++{std}" for std in STANDARDS],
        "overall": model.overall.as_dict(),
        "headers": [header.as_dict() for header in model.headers],
    }
    return json.dumps(payload, indent=2, sort_keys=False) + "\n"


def progress_cell(count: Count) -> str:
    if count.total == 0:
        return ""
    percent = count.percent
    assert percent is not None
    width = max(0.0, min(100.0, count.percent_float))
    return (
        '<div class="progress">'
        f'<span style="width: {width:.3f}%"></span>'
        "</div>"
        f'<strong>{html.escape(str(count.implemented))}/{html.escape(str(count.total))}</strong> '
        f"({percent}%)"
    )


def build_status_html(model: StatusModel) -> str:
    standard_cards = "\n".join(
        f"""
        <section class="metric">
          <h2>C++{std}</h2>
          <p>{progress_cell(model.overall.standards[std])}</p>
        </section>
        """
        for std in STANDARDS
    )

    header_rows = []
    for header in model.headers:
        if header.unavailable:
            total = "N/A"
        else:
            total = progress_cell(header.total)
        cells = [
            html.escape(header.name),
            total,
            progress_cell(header.standards[17]),
            progress_cell(header.standards[20]),
            progress_cell(header.standards[23]),
            progress_cell(header.standards[26]),
        ]
        header_rows.append(
            "<tr>" + "".join(f"<td>{cell}</td>" for cell in cells) + "</tr>"
        )

    return f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>STL Preview Status</title>
  <style>
    :root {{
      color-scheme: light dark;
      --bg: #f6f7f9;
      --fg: #17191f;
      --muted: #5d6675;
      --panel: #ffffff;
      --border: #d9dee7;
      --accent: #2870bd;
      --accent-soft: #d7e8fa;
    }}
    @media (prefers-color-scheme: dark) {{
      :root {{
        --bg: #111318;
        --fg: #f3f5f8;
        --muted: #a5adba;
        --panel: #1a1f27;
        --border: #313845;
        --accent: #72a7e8;
        --accent-soft: #203247;
      }}
    }}
    * {{ box-sizing: border-box; }}
    body {{
      margin: 0;
      background: var(--bg);
      color: var(--fg);
      font: 14px/1.5 system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
    }}
    main {{
      width: min(1180px, calc(100% - 32px));
      margin: 0 auto;
      padding: 32px 0 48px;
    }}
    header {{
      display: flex;
      align-items: end;
      justify-content: space-between;
      gap: 24px;
      margin-bottom: 24px;
    }}
    h1 {{
      margin: 0;
      font-size: 30px;
      line-height: 1.15;
      letter-spacing: 0;
    }}
    h2 {{
      margin: 0 0 10px;
      font-size: 14px;
      letter-spacing: 0;
      color: var(--muted);
    }}
    a {{ color: var(--accent); }}
    .links {{
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      justify-content: flex-end;
    }}
    .links a {{
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 6px 10px;
      background: var(--panel);
      text-decoration: none;
      white-space: nowrap;
    }}
    .metrics {{
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      gap: 12px;
      margin-bottom: 24px;
    }}
    .metric {{
      border: 1px solid var(--border);
      border-radius: 8px;
      background: var(--panel);
      padding: 14px;
    }}
    .metric p {{ margin: 0; }}
    .table-wrap {{
      overflow-x: auto;
      border: 1px solid var(--border);
      border-radius: 8px;
      background: var(--panel);
    }}
    table {{
      width: 100%;
      border-collapse: collapse;
      min-width: 860px;
    }}
    th, td {{
      border-bottom: 1px solid var(--border);
      padding: 10px 12px;
      text-align: left;
      vertical-align: middle;
    }}
    th {{
      color: var(--muted);
      font-weight: 650;
      background: color-mix(in srgb, var(--panel), var(--bg) 55%);
    }}
    tr:last-child td {{ border-bottom: 0; }}
    .progress {{
      display: inline-block;
      width: 92px;
      height: 8px;
      margin-right: 8px;
      border-radius: 4px;
      overflow: hidden;
      background: var(--accent-soft);
      vertical-align: middle;
    }}
    .progress span {{
      display: block;
      height: 100%;
      background: var(--accent);
    }}
    @media (max-width: 720px) {{
      main {{ width: min(100% - 20px, 1180px); padding-top: 20px; }}
      header {{ display: block; }}
      .links {{ justify-content: flex-start; margin-top: 14px; }}
    }}
  </style>
</head>
<body>
  <main>
    <header>
      <div>
        <h1>STL Preview Status</h1>
        <p>Implementation status generated from README detailed status tables.</p>
      </div>
      <nav class="links" aria-label="Reports">
        <a href="coverage/">Coverage</a>
        <a href="status.json">Status JSON</a>
        <a href="../README.md">README</a>
      </nav>
    </header>

    <section class="metrics">
      <section class="metric">
        <h2>Total</h2>
        <p>{progress_cell(model.overall.total)}</p>
      </section>
      {standard_cards}
    </section>

    <div class="table-wrap">
      <table>
        <thead>
          <tr>
            <th>Header</th>
            <th>Total</th>
            <th>C++17</th>
            <th>C++20</th>
            <th>C++23</th>
            <th>C++26</th>
          </tr>
        </thead>
        <tbody>
          {''.join(header_rows)}
        </tbody>
      </table>
    </div>
  </main>
</body>
</html>
"""


def write_if_changed(path: Path, content: str) -> bool:
    old_content = path.read_text(encoding="utf-8") if path.exists() else None
    if old_content == content:
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")
    return True


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def build_outputs(readme_path: Path) -> Tuple[str, str, str, StatusModel]:
    readme_text = read_text(readme_path)
    model = parse_readme_status(readme_text)
    updated_readme = replace_readme_status_block(
        readme_text,
        build_readme_status_block(model),
    )
    return (
        updated_readme,
        build_dashboard_markdown(model),
        build_dashboard_json(model),
        model,
    )


def check_file(path: Path, expected: str) -> bool:
    if not path.exists():
        print(f"{path} is missing", file=sys.stderr)
        return False
    actual = read_text(path)
    if actual != expected:
        print(f"{path} is stale; run tools/readme_gardener.py --write", file=sys.stderr)
        return False
    return True


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--readme", type=Path, default=Path("README.md"))
    parser.add_argument("--status-md", type=Path, default=Path("docs/status.md"))
    parser.add_argument("--status-json", type=Path, default=Path("docs/status.json"))
    parser.add_argument("--status-html", type=Path)
    parser.add_argument("--write", action="store_true", help="write generated files")
    parser.add_argument("--check", action="store_true", help="check generated files")
    args = parser.parse_args(argv)

    updated_readme, dashboard_md, dashboard_json, model = build_outputs(args.readme)

    if args.status_html:
        write_if_changed(args.status_html, build_status_html(model))
        write_if_changed(args.status_html.with_name("status.json"), dashboard_json)

    if args.write:
        changed = [
            write_if_changed(args.readme, updated_readme),
            write_if_changed(args.status_md, dashboard_md),
            write_if_changed(args.status_json, dashboard_json),
        ]
        if any(changed):
            print("Updated generated status files")
        else:
            print("Generated status files already up to date")
        return 0

    if args.check or not args.status_html:
        ok = all(
            [
                check_file(args.readme, updated_readme),
                check_file(args.status_md, dashboard_md),
                check_file(args.status_json, dashboard_json),
            ]
        )
        return 0 if ok else 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
