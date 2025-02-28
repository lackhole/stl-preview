set(HaveAtomicWaitAndNotify_Source "
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

function(HaveAtomicWaitAndNotify out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    PREVIEW_CHECK_CXX_SOURCE_COMPILES_UNTIL_CXX(20 "${HaveAtomicWaitAndNotify_Source}" ${out})
endfunction()
