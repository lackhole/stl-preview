set(HaveCXX20Atomic_Source "
#include <atomic>
#include <chrono>
#include <thread>

int main() {
    std::atomic<int> a{1};
    std::thread t1([&a] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        a.fetch_add(1);
        a.notify_one();
        a.notify_all();
    });
    a.wait(1);
    return 0;
}")

function(HaveCXX20Atomic out)
    PREVIEW_CHECK_COMPILES("${HaveCXX20Atomic_Source}" ${out})
    message("${out}: ${${out}}")
endfunction()
