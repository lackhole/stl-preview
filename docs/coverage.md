# Coverage Dashboard

The `Coverage Pages` workflow builds the test suite with GCC coverage flags on
pushes to `main` and on manual dispatch. It publishes a GitHub Pages site with:

| Page | Content |
|------|---------|
| `/` | Generated implementation status dashboard |
| `/coverage/` | gcovr HTML line coverage for `include/preview` |
| `/coverage/coverage.json` | gcovr JSON output for downstream tools |
| `/coverage/summary.txt` | gcovr text summary |

GitHub Pages must be configured to deploy from GitHub Actions.

## Local Coverage

```sh
python3 -m pip install --user gcovr

cmake -B build-coverage \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DPREVIEW_TEST=ON \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
  -S .

cmake --build build-coverage --config Debug -j 2
ctest --test-dir build-coverage --build-config Debug --output-on-failure

mkdir -p site/coverage
python3 tools/readme_gardener.py --status-html site/index.html
python3 -m gcovr -r . \
  --filter "$(pwd)/include/preview" \
  --exclude "$(pwd)/test" \
  --html --html-details \
  -o site/coverage/index.html
```
