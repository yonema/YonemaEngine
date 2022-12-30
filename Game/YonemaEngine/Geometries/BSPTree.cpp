#include "BSPTree.h"
#include "EigenJacobiMethod.h"

namespace nsYMEngine
{
	namespace nsGeometries
	{
		void CBSPTree::Build() noexcept
		{
			// ���[�g�m�[�h���쐬�B
			m_rootNode = CreateBSPTreeEntity(m_leafArray);
		}

		void CBSPTree::WalkTree(
			const nsMath::CVector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const noexcept
		{
			WalkTree(m_rootNode, pos, onEndWalk);
		}



		CBSPTree::SEntityPtr CBSPTree::CreateBSPTreeEntity(
            const std::vector<SEntityPtr>& leafArray) const noexcept
		{
            if (leafArray.size() == 1)
            {
                // ���[�t�G���e�B�e�B��Ԃ��B
                return leafArray.front();
            }

            // �听�����͂��s���āA�������ʂ����߂�B
            // �܂��́A���[�t�m�[�h�̒��S���W���v�Z����B
            nsMath::CVector3 centerPos = CalcCenterPositionFromLeafList(leafArray);

            // �����ċ����U�s����v�Z����
            float covarianceMatrix[3][3];
            CalcCovarianceMatrixFromLeafNodeList(covarianceMatrix, leafArray, centerPos);

            // �e�����U�̗v�f�����������Ă���B
            nsMath::CVector3* v_0 = (nsMath::CVector3*)covarianceMatrix[0];
            nsMath::CVector3* v_1 = (nsMath::CVector3*)covarianceMatrix[1];
            nsMath::CVector3* v_2 = (nsMath::CVector3*)covarianceMatrix[2];

            if (v_0->Length() < 0.1f && v_1->Length() < 0.1f && v_2->Length() < 0.1f)
            {
                // ���U���Ă��Ȃ��Ƃ������Ƃ́A�قƂ�ǂ̃��[�t�����ɋ߂��ꏊ�ɂ���Ƃ������ƂȂ̂ŁA
                // ����ȏ�̕����͍s��Ȃ��B
                // BSP�̖��[�m�[�h�Ƃ��āA���[�t�̔z��m�[�h���쐬����B
                return CreateBSPTreeEntity_LeafList(leafArray);
            }

            // �V�����m�[�h�����B
            auto newNodePtr = std::make_shared<SNode>();
            newNodePtr->type = EnEntityType::enNode;
            newNodePtr->centerPos = centerPos;
            newNodePtr->leafArray = leafArray;
            auto* newNode = static_cast<SNode*>(newNodePtr.get());

            // ���U���Ă���̂ŁA�����U�s��𗘗p����
            // �������ʂ��v�Z����B
            CalcSplitPlaneFromCovarianceMatrix(newNode->plane, covarianceMatrix, leafArray, centerPos);

            // �������ʂ����܂����̂ŁA���[�t�𕽖ʂŐU�蕪�����Ă����B
            std::vector<SEntityPtr> leftLeafArray;
            std::vector<SEntityPtr> rightLeafArray;
            SplitLeafArray(leftLeafArray, rightLeafArray, newNode->plane, leafArray);


            if (leftLeafArray.empty() || rightLeafArray.empty())
            {
                // �Е��̎}����ɂȂ����B
                // ���U���Ă���̂ŁA���ɂ͗��Ȃ��͂��Ȃ񂾂��ǁA�����ꗈ�Ă��܂��ƁA�ċN�Ăяo�����I��炸��
                // �X�^�b�N�I�[�o�[�t���[���Ă��܂��̂ŁA�ی��Ƃ��āB
                // ���U���Ă��Ȃ��̂Ń��[�t�̃��X�g�m�[�h���쐬����B
                return CreateBSPTreeEntity_LeafList(leafArray);
            }

            // ���̎}���\�z
            newNode->leftEntity = CreateBSPTreeEntity(leftLeafArray);

            // �E�̎}���\�z�B
            newNode->rightEntity = CreateBSPTreeEntity(rightLeafArray);


            return newNodePtr;
		}

        CBSPTree::SEntityPtr CBSPTree::CreateBSPTreeEntity_LeafList(
            const std::vector<SEntityPtr>& leafArray) const noexcept
        {
            auto newNodePtr = std::make_shared<SLeafList>();
            auto* leafList = static_cast<SLeafList*>(newNodePtr.get());
            leafList->type = EnEntityType::enLeafList;
            leafList->leafList = leafArray;
            return newNodePtr;
        }

        nsMath::CVector3 CBSPTree::CalcCenterPositionFromLeafList(
            const std::vector<SEntityPtr>& leafArray) const noexcept
        {
            // �܂��́AAABB�̒��S���W�����߂�B
            nsMath::CVector3 centerPos;
            for (const auto& leafPtr : leafArray)
            {
                auto* leaf = static_cast<SLeaf*>(leafPtr.get());
                centerPos += leaf->position;
            }
            centerPos /= static_cast<float>(leafArray.size());
            return centerPos;
        }

        void CBSPTree::CalcCovarianceMatrixFromLeafNodeList(
            float covarianceMatrix[3][3],
            const std::vector<SEntityPtr>& leafNodeArray,
            const nsMath::CVector3& centerPos
        ) const noexcept
        {
            // 0�ŏ���������B
            memset(covarianceMatrix, 0, sizeof(float[3][3]));

            // �����U�s����v�Z����B
            // �����U�Ƃ�X��Y��Z�̗v�f���ǂꂭ�炢�֘A�Â��ĕ��U���Ă��邩��\�����́B
            // �����U�s��́A������s��Ƃ��Ă܂Ƃ߂����́B
            for (const auto& leafPtr : leafNodeArray)
            {
                auto* leaf = static_cast<SLeaf*>(leafPtr.get());
                const auto& aabbCenterPos = leaf->position;
                covarianceMatrix[0][0] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.x - centerPos.x);
                covarianceMatrix[0][1] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.y - centerPos.y);
                covarianceMatrix[1][0] = covarianceMatrix[0][1];

                covarianceMatrix[1][1] += (aabbCenterPos.y - centerPos.y) * (aabbCenterPos.y - centerPos.y);
                covarianceMatrix[0][2] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.z - centerPos.z);
                covarianceMatrix[2][0] = covarianceMatrix[0][2];

                covarianceMatrix[2][2] += (aabbCenterPos.z - centerPos.z) * (aabbCenterPos.z - centerPos.z);
                covarianceMatrix[1][2] += (aabbCenterPos.y - centerPos.y) * (aabbCenterPos.z - centerPos.z);
                covarianceMatrix[2][1] = covarianceMatrix[1][2];
            }

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    covarianceMatrix[i][j] /= static_cast<float>(m_leafArray.size());
                }
            }

            return;
        }

        void CBSPTree::CalcSplitPlaneFromCovarianceMatrix(
            SPlane& plane,
            float covarianceMatrix[3][3],
            const std::vector<SEntityPtr>& leafArray,
            const nsMath::CVector3& centerPos
        ) const noexcept
        {
            // �����U�s�񂪌v�Z�ł����̂ŁA���R�r�@��p���ČŗL�l�ƌŗL�x�N�g�������߂�B
            nsMath::CVector3 eigenVector[3];
            EigenJacobiMethod<3>(
                reinterpret_cast<float*>(covarianceMatrix),
                reinterpret_cast<float*>(&eigenVector)
                );

            // 1�Ԗڑ傫�ȌŗL�l�̌ŗL�x�N�g���𕪊����ʂ̖@���Ƃ���B
            float eigenScalar_0 = fabsf(covarianceMatrix[0][0]);
            float eigenScalar_1 = fabsf(covarianceMatrix[1][1]);
            float eigenScalar_2 = fabsf(covarianceMatrix[2][2]);

            if (eigenScalar_0 > eigenScalar_1 && eigenScalar_0 > eigenScalar_2)
            {
                plane.normal.x = eigenVector[0].x;
                plane.normal.y = eigenVector[1].x;
                plane.normal.z = eigenVector[2].x;
            }
            else if (eigenScalar_1 > eigenScalar_0 && eigenScalar_1 > eigenScalar_2)
            {
                plane.normal.x = eigenVector[0].y;
                plane.normal.y = eigenVector[1].y;
                plane.normal.z = eigenVector[2].y;
            }
            else if (eigenScalar_2 > eigenScalar_0 && eigenScalar_2 > eigenScalar_1)
            {
                plane.normal.x = eigenVector[0].z;
                plane.normal.y = eigenVector[1].z;
                plane.normal.z = eigenVector[2].z;
            }
            if (plane.normal.Length() < 0.1f)
            {
                // ���R�r�@�Ŗ@�����v�Z�ł��Ȃ������B

                SLeaf* leafFront = static_cast<SLeaf*>(leafArray.front().get());
                SLeaf* leafBack = static_cast<SLeaf*>(leafArray.back().get());

                plane.normal = leafBack->position - leafFront->position;

                plane.normal.Normalize();
            }
            // �������ʂ܂ł̋����͒��S���W�܂ł̋����Ƃ���B
            plane.distance = Dot(plane.normal, centerPos);
            if (plane.distance < 0.0f)
            {
                plane.normal *= -1.0f;
                plane.distance = fabsf(plane.distance);
            }

            return;
        }

        void CBSPTree::SplitLeafArray(
            std::vector<SEntityPtr>& leftLeafArray,
            std::vector<SEntityPtr>& rightLeafArray,
            const SPlane& plane,
            const std::vector<SEntityPtr>& leafArray
        ) const noexcept
        {
            for (const auto& leafPtr : leafArray)
            {
                auto leaf = static_cast<SLeaf*>(leafPtr.get());
                float t = Dot(leaf->position, plane.normal);

                if (t < plane.distance)
                {
                    // �����Ɋ���U��B
                    leftLeafArray.emplace_back(leafPtr);
                }
                else
                {
                    // �E���Ɋ���U��B
                    rightLeafArray.emplace_back(leafPtr);
                }

            }

            return;
        }


        void CBSPTree::WalkTree(
            SEntityPtr entityPtr,
            const nsMath::CVector3& pos,
            std::function<void(SLeaf* leaf)> onEndWalk
        ) const noexcept
        {
            if (entityPtr->type == EnEntityType::enNode)
            {
                // ����̓m�[�h�Ȃ̂ł���ɐ���B
                // ���ɐ���H�E�ɐ���H
                SNode* node = static_cast<SNode*>(entityPtr.get());
                //for (SEntityPtr& entity : node->leafArray)
                //{

                //}
                float t = Dot(pos, node->plane.normal);
                if (t < node->plane.distance)
                {
                    // ���ɐ���B
                    WalkTree(node->leftEntity, pos, onEndWalk);
                }
                else
                {
                    // �E�ɐ���B
                    WalkTree(node->rightEntity, pos, onEndWalk);
                }
            }
            else if (entityPtr->type == EnEntityType::enLeaf)
            {
                // ���[�t�ɓ��B�����B
                onEndWalk(static_cast<SLeaf*>(entityPtr.get()));
            }
            else if (entityPtr->type == EnEntityType::enLeafList)
            {
                // ���[�t�̃��X�g�ɓ��B�����B
                auto leafList = static_cast<SLeafList*>(entityPtr.get());
                for (auto leaf : leafList->leafList)
                {
                    onEndWalk(static_cast<SLeaf*>(leaf.get()));
                }
            }

            return;
        }


	}
}