#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CPipelineState : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CPipelineState() = default;
				~CPipelineState();

				bool InitAsGraphicsPipelineState(
					const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
					const wchar_t* objectName = nullptr
				);

				void Release();

				constexpr auto Get() noexcept
				{
					return m_pipelineState;
				}

				void SetName(const wchar_t* objectName);



			private:
				void Terminate();

			private:
				ID3D12PipelineState* m_pipelineState = nullptr;

			};

		}
	}
}