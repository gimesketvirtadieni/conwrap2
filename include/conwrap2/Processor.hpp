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

#include <conwrap2/ProcessorImpl.hpp>
#include <memory>


namespace conwrap2
{
	template <typename ResourceType>
	class Processor
	{
		public:
			Processor(ResourceType resource)
			: processorImplPtr{std::make_unique<internal::ProcessorImpl<ResourceType>>(std::move(resource))}
			{
				processorImplPtr->start();
			}

			template <typename CreatorType = std::function<ResourceType(ProcessorProxy<ResourceType>&)>>
			Processor(CreatorType&& creator)
			: processorImplPtr{std::make_unique<internal::ProcessorImpl<ResourceType>>(creator)}
			{
				processorImplPtr->start();
			}

			~Processor()
			{
				processorImplPtr->stop();
			}

			Processor(const Processor&) = delete;             // non-copyable
			Processor& operator=(const Processor&) = delete;  // non-assignable
			Processor(Processor&& rhs) = default;
			Processor& operator=(Processor&& rhs) = default;

			inline auto& getDispatcher()
			{
				return processorImplPtr->getDispatcher();
			}

			inline auto& getResource()
			{
				return processorImplPtr->getResource();
			}

			template<typename HandlerType>
			inline void process(HandlerType&& handler)
			{
				processorImplPtr->process(handler);
			}

			template<typename HandlerType, typename DurationType>
			inline auto& processWithDelay(HandlerType&& handler, const DurationType& delay)
			{
				return processorImplPtr->processWithDelay(handler, delay);
			}

		private:
			std::unique_ptr<internal::ProcessorImpl<ResourceType>> processorImplPtr;
	};
}
