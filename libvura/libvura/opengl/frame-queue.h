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

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
extern "C" {
#include <libavutil/frame.h>
}

class FrameQueue {
public:
    explicit FrameQueue(int max_size = 15);
    ~FrameQueue();

    void abort();
    void reset();
    bool push(AVFrame* frame);
    AVFrame* tryPop();
    void clear();

    /**
     * Returns the PTS of the front frame WITHOUT removing it.
     * Returns AV_NOPTS_VALUE if the queue is empty.
     * Used by VideoWidget to decide whether it's time to display
     * the next frame without committing to popping it yet.
     */
    int64_t peekPts() const;

private:
    std::queue<AVFrame*> queue;
    mutable std::mutex mutex;   // mutable so peekPts() can lock in a const method
    std::condition_variable cond;
    bool abortRequested = false;
    int maxSize = 15;

};
