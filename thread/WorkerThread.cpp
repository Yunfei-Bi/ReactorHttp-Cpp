#include "WorkerThread.h"
#include <stdio.h>

// 子线程的回调函数
void WorkerThread::running()
{
    m_mutex.lock();
    m_evLoop = new EventLoop(m_name);
    m_mutex.unlock();
    m_cond.notify_one();
    // 这里的run()里面的dispatch()需要的回调函数 readCallback 等, 
    // 需要指向 processRead, processWrite, destroy
    m_evLoop->run();
}

WorkerThread::WorkerThread(int index)
{
    m_evLoop = nullptr;
    m_thread = nullptr;
    // thread::id()作用是将其设置为无效状态, 这通常用于清理或标记线程未启动、已完成，或者解绑线程
    m_threadID = thread::id(); 
    m_name =  "SubThread-" + to_string(index);
}

WorkerThread::~WorkerThread()
{ 
    if (m_thread != nullptr)
    {
        delete m_thread;
    }
}

void WorkerThread::run()
{
    // 创建子线程
    m_thread = new thread(&WorkerThread::running, this);
    // 阻塞主线程, 让当前函数不会直接结束
    unique_lock<mutex> locker(m_mutex);
    while (m_evLoop == nullptr)
    {
        m_cond.wait(locker);   
    }
}
