#include "BSPTree.h"
#include "EigenJacobiMethod.h"

namespace nsYMEngine
{
	namespace nsGeometries
	{
		void CBSPTree::Build() noexcept
		{
			// ルートノードを作成。
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
                // リーフエンティティを返す。
                return leafArray.front();
            }

            // 主成分分析を行って、分割平面を求める。
            // まずは、リーフノードの中心座標を計算する。
            nsMath::CVector3 centerPos = CalcCenterPositionFromLeafList(leafArray);

            // 続いて共分散行列を計算する
            float covarianceMatrix[3][3];
            CalcCovarianceMatrixFromLeafNodeList(covarianceMatrix, leafArray, centerPos);

            // 各共分散の要素を引っ張ってくる。
            nsMath::CVector3* v_0 = (nsMath::CVector3*)covarianceMatrix[0];
            nsMath::CVector3* v_1 = (nsMath::CVector3*)covarianceMatrix[1];
            nsMath::CVector3* v_2 = (nsMath::CVector3*)covarianceMatrix[2];

            if (v_0->Length() < 0.1f && v_1->Length() < 0.1f && v_2->Length() < 0.1f)
            {
                // 分散していないということは、ほとんどのリーフが非常に近い場所にあるということなので、
                // これ以上の分割は行わない。
                // BSPの末端ノードとして、リーフの配列ノードを作成する。
                return CreateBSPTreeEntity_LeafList(leafArray);
            }

            // 新しいノードを作る。
            auto newNodePtr = std::make_shared<SNode>();
            newNodePtr->type = EnEntityType::enNode;
            newNodePtr->centerPos = centerPos;
            newNodePtr->leafArray = leafArray;
            auto* newNode = static_cast<SNode*>(newNodePtr.get());

            // 分散しているので、共分散行列を利用して
            // 分割平面を計算する。
            CalcSplitPlaneFromCovarianceMatrix(newNode->plane, covarianceMatrix, leafArray, centerPos);

            // 分割平面が求まったので、リーフを平面で振り分けしていく。
            std::vector<SEntityPtr> leftLeafArray;
            std::vector<SEntityPtr> rightLeafArray;
            SplitLeafArray(leftLeafArray, rightLeafArray, newNode->plane, leafArray);


            if (leftLeafArray.empty() || rightLeafArray.empty())
            {
                // 片方の枝が空になった。
                // 分散しているので、こには来ないはずなんだけど、万が一来てしまうと、再起呼び出しが終わらずに
                // スタックオーバーフローしてしまうので、保険として。
                // 分散していないのでリーフのリストノードを作成する。
                return CreateBSPTreeEntity_LeafList(leafArray);
            }

            // 左の枝を構築
            newNode->leftEntity = CreateBSPTreeEntity(leftLeafArray);

            // 右の枝を構築。
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
            // まずは、AABBの中心座標を求める。
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
            // 0で初期化する。
            memset(covarianceMatrix, 0, sizeof(float[3][3]));

            // 共分散行列を計算する。
            // 共分散とはXとYとZの要素がどれくらい関連づいて分散しているかを表すもの。
            // 共分散行列は、それを行列としてまとめたもの。
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
            // 共分散行列が計算できたので、ヤコビ法を用いて固有値と固有ベクトルを求める。
            nsMath::CVector3 eigenVector[3];
            EigenJacobiMethod<3>(
                reinterpret_cast<float*>(covarianceMatrix),
                reinterpret_cast<float*>(&eigenVector)
                );

            // 1番目大きな固有値の固有ベクトルを分割平面の法線とする。
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
                // ヤコビ法で法線が計算できなかった。

                SLeaf* leafFront = static_cast<SLeaf*>(leafArray.front().get());
                SLeaf* leafBack = static_cast<SLeaf*>(leafArray.back().get());

                plane.normal = leafBack->position - leafFront->position;

                plane.normal.Normalize();
            }
            // 分割平面までの距離は中心座標までの距離とする。
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
                    // 左側に割り振る。
                    leftLeafArray.emplace_back(leafPtr);
                }
                else
                {
                    // 右側に割り振る。
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
                // これはノードなのでさらに潜る。
                // 左に潜る？右に潜る？
                SNode* node = static_cast<SNode*>(entityPtr.get());
                //for (SEntityPtr& entity : node->leafArray)
                //{

                //}
                float t = Dot(pos, node->plane.normal);
                if (t < node->plane.distance)
                {
                    // 左に潜る。
                    WalkTree(node->leftEntity, pos, onEndWalk);
                }
                else
                {
                    // 右に潜る。
                    WalkTree(node->rightEntity, pos, onEndWalk);
                }
            }
            else if (entityPtr->type == EnEntityType::enLeaf)
            {
                // リーフに到達した。
                onEndWalk(static_cast<SLeaf*>(entityPtr.get()));
            }
            else if (entityPtr->type == EnEntityType::enLeafList)
            {
                // リーフのリストに到達した。
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