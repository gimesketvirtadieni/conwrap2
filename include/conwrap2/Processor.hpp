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

#include <conwrap2/ProcessorImpl.hpp>


namespace conwrap2
{
	template <typename ResourceType>
	class Processor
	{
		public:
			Processor(ResourceType resource)
			: processorImpl{std::move(resource)} {}

			template <typename CreatorType = std::function<ResourceType(ProcessorProxy<ResourceType>&)>>
			Processor(CreatorType&& creator)
			: processorImpl{creator}
			{
				processorImpl.start();
			}

			~Processor()
			{
				processorImpl.stop();
			}

			inline auto& getDispatcher()
			{
				return processorImpl.getDispatcher();
			}

			inline auto& getResource()
			{
				return processorImpl.getResource();
			}

			template<typename Func>
			inline void process(Func&& handler)
			{
				processorImpl.process(handler);
			}

		private:
			internal::ProcessorImpl<ResourceType> processorImpl;
	};
}
