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

#include "packet-queue.h"


AVPacket* PacketQueue::getFlushPacket()
{
    static AVPacket* packet = []() {
        AVPacket* p = av_packet_alloc();
        return p;
    }();
    return packet;
}

PacketQueue::PacketQueue(int max_size) : maxSize(max_size) {}

PacketQueue::~PacketQueue()
{
    clear();
}

void PacketQueue::abort()
{
    std::lock_guard<std::mutex> lock(mutex);
    abortRequested = true;
    cond.notify_all();
}

bool PacketQueue::push(AVPacket* pkt)
{
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this]() { return queue.size() < maxSize || abortRequested; });

    if (abortRequested) {
        return false;
    }

    queue.push(pkt);
    cond.notify_one();
    return true;
}

AVPacket* PacketQueue::pop()
{
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [this]() { return !queue.empty() || abortRequested; });

    if (abortRequested || queue.empty()) {
        return nullptr;
    }

    AVPacket* pkt = queue.front();
    queue.pop();
    cond.notify_one();
    return pkt;
}

void PacketQueue::clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    while (!queue.empty()) {
        AVPacket* pkt = queue.front();
        queue.pop();
        if (pkt != getFlushPacket()) {
            av_packet_free(&pkt);
        }
    }
    cond.notify_all();
}

void PacketQueue::reset()
{
    std::lock_guard<std::mutex> lock(mutex);
    while (!queue.empty()) {
        AVPacket* pkt = queue.front();
        queue.pop();
        if (pkt != getFlushPacket())
            av_packet_free(&pkt);
    }
    abortRequested = false;
    cond.notify_all();
}
