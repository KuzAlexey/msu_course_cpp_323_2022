#include <atomic>
#include <cassert>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

#include "graph_generation_controller.hpp"

namespace uni_course_cpp {
void GraphGenerationController::Worker::start() {
  assert(state_ == State::Idle);

  state_ = State::Working;

  thread_ = std::thread([this]() {
    while (true) {
      if (state_ == State::ShouldTerminate) {
        return;
      }

      const auto job_optional = get_job_callback_();
      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  });
}

void GraphGenerationController::Worker::stop() {
  if (state_ == State::Working) {
    state_ = State::ShouldTerminate;
    thread_.join();
  }
}

GraphGenerationController::Worker::~Worker() {
  if (state_ == State::ShouldTerminate) {
    stop();
  }
}

GraphGenerationController::GraphGenerationController(
    int threads_count,
    int graphs_count,
    GraphGenerator::Params&& graph_generator_params)
    : threads_count_(threads_count),
      graphs_count_(graphs_count),
      graph_generator_(std::move(graph_generator_params)) {
  const auto job_optional = [this]() -> std::optional<JobCallback> {
    const std::lock_guard lock(jobs_mutex_);

    if (!jobs_.empty()) {
      auto job = jobs_.back();
      jobs_.pop_back();

      return job;
    }
    return std::nullopt;
  };

  for (int i = 0; i < threads_count_; i++) {
    workers_.emplace_back(job_optional);
  }
}

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::mutex jobs_mutex;

  std::atomic<int> current_jobs_count = graphs_count_;
  for (int i = 0; i < graphs_count_; i++) {
    jobs_.emplace_back([i, &gen_started_callback, &gen_finished_callback,
                        &current_jobs_count, &jobs_mutex, this]() {
      const std::lock_guard lock(jobs_mutex);
      gen_started_callback(i);
      auto graph = graph_generator_.generate();
      gen_finished_callback(i, std::move(graph));

      current_jobs_count--;
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (current_jobs_count > 0) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}
}  // namespace uni_course_cpp
