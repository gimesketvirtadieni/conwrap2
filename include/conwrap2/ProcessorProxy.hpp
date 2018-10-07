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


namespace conwrap2
{
	namespace internal
	{
		template <typename ResourceType>
		class ProcessorImpl;
	}

	template <typename ResourceType>
	class ProcessorProxy
	{
		public:
			ProcessorProxy(internal::ProcessorImpl<ResourceType>& p)
			: processorImpl{p} {}

			inline auto& getDispatcher()
			{
				return processorImpl.getDispatcher();
			}

			inline auto& getResource()
			{
				return processorImpl.getResource();
			}

			template<typename HandlerType>
			inline void process(HandlerType&& handler)
			{
				processorImpl.process(std::move(handler));
			}

			template<typename HandlerType, typename DurationType>
			inline auto& processWithDelay(HandlerType&& handler, const DurationType& delay)
			{
				return processorImpl.processWithDelay(handler, delay);
			}

		private:
			internal::ProcessorImpl<ResourceType>& processorImpl;
	};
}
