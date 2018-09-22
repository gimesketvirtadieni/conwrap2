/*
 * Copyright 2016, Andrej Kislovskij
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

#include <experimental/net>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>

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

				inline auto& getDispatcher()
				{
					return io_context;
				}

				inline auto& getResource()
				{
					return resource;
				}

				template<typename Func>
				inline void process(Func&& handler)
				{
					Context<ResourceType> context{processorProxy};

					if constexpr (std::is_invocable<decltype(handler), decltype((context))>::value)
					{
						net::post(io_context, std::move([handler{std::move(handler)}, context{std::move(context)}]
						{
							handler(context);
						}));
					}
					else
					{
						net::post(io_context, std::move(handler));
					}
				}

				inline void start()
				{
					std::lock_guard<std::mutex> lockGuard(threadLock);

					if (!thread.joinable())
					{
						thread = std::thread([&]
						{
							io_context.restart();
							workGuard.emplace(net::make_work_guard(io_context));
							io_context.run();
						});
					}
				}

				inline void stop()
				{
					std::lock_guard<std::mutex> lockGuard(threadLock);

					if (thread.joinable())
					{
						// it will make io_context.run() exit as soon as there is no tasks to process
						workGuard.reset();

						// waiting until processor's thread finsihes
						thread.join();
					}
				}

			private:
				ProcessorProxy<ResourceType> processorProxy;
				ResourceType                 resource;
				std::thread                  thread;
				std::mutex                   threadLock;
				net::io_context              io_context;
				std::optional<WorkGuardType> workGuard{std::nullopt};
		};
	}
}
