/* SPDX-License-Identifier: MIT */

/* StACSOS - Kernel
 *
 * Copyright (c) University of St Andrews 2024
 * Tom Spink <tcs6@st-andrews.ac.uk>
 */
#include <stacsos/kernel/sched/alg/rr.h>

// *** COURSEWORK NOTE *** //
// This will be where you are implementing your round-robin scheduling
// algorithm. Please edit this file in any way you see fit.  You may also edit
// the rr.h header file.

using namespace stacsos::kernel::sched;
using namespace stacsos::kernel::sched::alg;

void round_robin::add_to_runqueue(tcb &tcb) {
  // Adds a thread to the queue
  runqueue_.append(&tcb);
}

void round_robin::remove_from_runqueue(tcb &tcb) {
  // Removes a thread from the queue
  runqueue_.remove(&tcb);
}

tcb *round_robin::select_next_task(tcb *current) {
  // If queue is empty, return a null pointer
  if (runqueue_.empty()) {
    return nullptr;
  }

  // Takes the first element from the queue
  tcb *candidate = runqueue_.dequeue();

  // Enqueues first element to the back of the queue
  runqueue_.enqueue(candidate);

  // Returns current thread to be run
  return candidate;
}
