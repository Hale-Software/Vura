/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "frame-queue.h"

FrameQueue::FrameQueue(int max_size) : maxSize(max_size) {}

FrameQueue::~FrameQueue()
{
    clear();
}

void FrameQueue::abort()
{
    std::lock_guard<std::mutex> lock(mutex);
    abortRequested = true;
    cond.notify_all();
}

void FrameQueue::reset()
{
    std::lock_guard<std::mutex> lock(mutex);
    while (!queue.empty()) {
        AVFrame* frame = queue.front();
        av_frame_free(&frame);
        queue.pop();
    }
    abortRequested = false;
    cond.notify_all();
}

bool FrameQueue::push(AVFrame* frame)
{
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this]() { return queue.size() < static_cast<size_t>(maxSize) || abortRequested; });

    if (abortRequested)
        return false;

    queue.push(frame);
    cond.notify_one();
    return true;
}

AVFrame* FrameQueue::tryPop()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty() || abortRequested) return nullptr;
    AVFrame* frame = queue.front();
    queue.pop();
    cond.notify_one();
    return frame;
}

int64_t FrameQueue::peekPts() const
{
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty()) return AV_NOPTS_VALUE;
    return queue.front()->pts;
}

void FrameQueue::clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    while (!queue.empty()) {
        AVFrame* frame = queue.front();
        av_frame_free(&frame);
        queue.pop();
    }
    cond.notify_all();
}
