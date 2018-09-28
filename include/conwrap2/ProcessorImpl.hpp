/*
 * Copyright 2018, Andrej Kislovskij
 *
 * This is PUBLIC DOMAIN software so use at your own risk as it comes
 * with no warranties. This code is yours to share, use and modify without
 * any restrictions or obligations.
 *
 * For more information see conwrap/LICENSE or refer refer to http://unlicense.org
 *
 * Author: gimesketvirtadieni at gmail dot com (Andrej Kislovskij)
 */

#pragma once

#include <algorithm>
#include <experimental/net>
#include <functional>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>  // std::pair
#include <vector>

#include <conwrap2/Context.hpp>
#include <conwrap2/ProcessorProxy.hpp>


namespace conwrap2
{
	namespace internal
	{
		namespace net = std::experimental::net;

		template <typename ResourceType>
		class ProcessorImpl
		{
			using WorkGuardType = net::executor_work_guard<net::io_context::executor_type>;
			using TimerType     = net::high_resolution_timer;

			public:
				ProcessorImpl(ResourceType r)
				: processorProxy{*this}
				, resource{std::move(r)} {}

				template <typename CreatorType = std::function<ResourceType(ProcessorProxy<ResourceType>&)>>
				ProcessorImpl(CreatorType&& creator)
				: processorProxy{*this}
				, resource{creator(processorProxy)} {}

				~ProcessorImpl()
				{
					// stopping this processor in case it was left running
					stop();
				}

				ProcessorImpl(const ProcessorImpl&) = delete;             // non-copyable
				ProcessorImpl& operator=(const ProcessorImpl&) = delete;  // non-assignable
				ProcessorImpl(ProcessorImpl&& rhs) = delete;              // non-movable
				ProcessorImpl& operator=(ProcessorImpl&& rhs) = delete;   // non-move-assignable

				inline auto& getDispatcher()
				{
					return dispatcher;
				}

				inline auto& getResource()
				{
					return resource;
				}

				template<typename HandlerType>
				inline void process(HandlerType&& handler)
				{
					net::post(dispatcher, std::move(wrap(handler)));
				}

				template<typename HandlerType, typename DurationType>
				inline void processWithDelay(HandlerType&& handler, const DurationType& delay)
				{
					process(std::move([this, handler{std::move(wrap(handler))}, delay{delay}](auto& context) mutable
					{
						auto& [id, timer]{*timers.emplace(timers.end(), ++currentID, std::move(TimerType{dispatcher, delay}))};

						timer.async_wait(std::move([this, context{context}, handler{std::move(handler)}, id{id}](auto& error) mutable
						{
							if (!error)
							{
								handler();
							}

							context.getProcessorProxy().process([this, id{id}]() mutable
							{
								timers.erase(std::remove_if(timers.begin(), timers.end(), [expiredID{id}](auto& pair)
								{
									auto& [id, timer]{pair};
									return id == expiredID;
								}));
							});
						}));
					}));
				}

				inline void start()
				{
					std::lock_guard<std::mutex> lockGuard{threadLock};

					if (!thread.joinable())
					{
						thread = std::thread([&]
						{
							dispatcher.restart();
							workGuard.emplace(net::make_work_guard(dispatcher));
							dispatcher.run();
						});
					}
				}

				inline void stop()
				{
					std::lock_guard<std::mutex> lockGuard{threadLock};

					if (thread.joinable())
					{
						process([this]
						{
							for (auto& [id, timer] : timers)
							{
								timer.cancel();
							}

							// it will make dispatcher.run() exit as soon as there is no tasks to process
							workGuard.reset();
						});

						// waiting until processor's thread finsihes
						thread.join();
					}
				}

			protected:
				template<typename HandlerType>
				inline auto wrap(HandlerType&& handler)
				{
					Context<ResourceType> context{processorProxy};

					if constexpr (std::is_invocable<decltype(handler), decltype((context))>::value)
					{
						return std::move([handler{std::move(handler)}, context{std::move(context)}]() mutable
						{
							handler(context);
						});
					}
					else
					{
						return std::move(handler);
					}
				}

			private:
				ProcessorProxy<ResourceType>                          processorProxy;
				ResourceType                                          resource;
				std::thread                                           thread;
				std::mutex                                            threadLock;
				net::io_context                                       dispatcher;
				std::optional<WorkGuardType>                          workGuard{std::nullopt};
				std::vector<std::pair<unsigned long long, TimerType>> timers;
				unsigned long long                                    currentID{0};
		};
	}
}
