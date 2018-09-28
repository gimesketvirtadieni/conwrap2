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
	// forward declaration
	template <typename ResourceType>
	class ProcessorProxy;

	template <typename ResourceType>
	class Context
	{
		public:
			Context(ProcessorProxy<ResourceType>& p)
			: processorProxy(p) {}

			inline auto& getDispatcher()
			{
				return processorProxy.getDispatcher();
			}

			inline auto& getProcessorProxy()
			{
				return processorProxy;
			}

			inline auto& getResource()
			{
				return processorProxy.getResource();
			}

		private:
			ProcessorProxy<ResourceType>& processorProxy;
	};
}
