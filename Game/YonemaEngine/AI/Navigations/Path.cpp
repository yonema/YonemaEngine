#include "Path.h"
namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			nsMath::CVector3 CPath::Move(
				const nsMath::CVector3& targetPos,
				float moveSpeed,
				float deltaTime,
				bool& isEnd,
				bool enablePhysics
			)
			{
				if (m_sectionArray.empty() || m_sectionNo >= m_sectionArray.size())
				{
					// �p�X���\�z����Ă��Ȃ�
					return targetPos;
				}

				SSection& currentSection = m_sectionArray.at(m_sectionNo);
				// �Z�N�V�����̏I�_�Ɍ������x�N�g�����v�Z����B
				nsMath::CVector3 toEnd = currentSection.endPos - targetPos;
				toEnd.Normalize();
				nsMath::CVector3 movePos = targetPos + toEnd * moveSpeed * deltaTime;

				nsMath::CVector3 toEnd2 = currentSection.endPos - movePos;
				toEnd2.Normalize();

				if (toEnd.Dot(toEnd2) <= 0.0f)
				{
					// �������ς�����̂ŏI�_�𒴂����B
					movePos = currentSection.endPos;
					if (m_sectionNo == m_sectionArray.size() - 1)
					{
						// �I�_
						isEnd = true;
					}
					else
					{
						m_sectionNo++;
					}
				}

				if (enablePhysics)
				{
					nsMath::CVector3 rayStart = movePos;
					// ������Ə�B
					rayStart.y += 1.0f;
					nsMath::CVector3 rayEnd = rayStart;
					rayEnd.y -= 1000.0f;
					nsMath::CVector3 hitPos;
					//if (physicsWorld->RayTest(rayStart, rayEnd, hitPos))
					//{
					//	movePos = hitPos;
					//}
				}


				return movePos;
			}


			void CPath::Build() noexcept
			{
				m_sectionArray.resize(m_pointArray.size() - 1);

				// �Z�N�V�������\�z���Ă����B
				for (int pointNo = 0; pointNo < m_pointArray.size() - 1; pointNo++)
				{
					auto& section = m_sectionArray.at(pointNo);
					section.startPos = m_pointArray.at(pointNo);
					section.endPos = m_pointArray.at(pointNo + 1);
					section.direction = section.endPos - section.startPos;
					section.length = section.direction.Length();
					section.direction.Normalize();
				}

				return;
			}




		}
	}
}
