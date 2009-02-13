/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRTouchSensor.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

void CRTouchSensor::Init() {
	CRSensor::Init();
}

void CRTouchSensor::Step() {
	CRBodyIf* body = creature->GetBody(0);

	// 接触リストの構築を開始する
	contactList.clear();

	int sceneCnt = phScene->GetCount();

	PHSolidPairForLCPIf* solidPair;
	PHShapePairForLCPIf* shapePair;

	Vec3d totalForce;

	// PHScene中のすべての剛体についてチェック
	for (int i=0; i<phScene->NSolids(); ++i) {
		for (int j=i+1; j<phScene->NSolids(); ++j) {
			// 自分の体を構成する剛体 と それ以外の剛体 のペアのみに限定
			bool iIsMe = false, jIsMe = false;
			for (int n=0; n<body->NSolids(); ++n) {
				if (body->GetSolid(n) == phScene->GetSolids()[i]) { iIsMe = true; }
				if (body->GetSolid(n) == phScene->GetSolids()[j]) { jIsMe = true; }
				if (iIsMe && jIsMe) { break; }
			}
			if ((iIsMe && jIsMe) || (!iIsMe && !jIsMe)) { continue; }

			PHSolidIf *so1, *so2, *soMe, *soOther;
			if (iIsMe) {
				so1 = soMe		= phScene->GetSolids()[i];
				so2 = soOther	= phScene->GetSolids()[j];
			} else {
				so1 = soOther	= phScene->GetSolids()[i];
				so2 = soMe		= phScene->GetSolids()[j];
			}

			solidPair = phScene->GetSolidPair(i, j);
			PHConstraintIf* constraint = phScene->GetConstraintEngine()->GetContactPoints()->FindBySolidPair(so1, so2);

			if (!solidPair)  { continue; }
			if (!constraint) { continue; }

			Vec3d force = phScene->GetConstraintEngine()->GetContactPoints()->GetTotalForce(so1, so2);

			for (int s=0; s<so1->NShape(); ++s) {
				for (int t=s+1; t<so2->NShape(); ++t) {

					// この方法だと同じ剛体の異なるShapeについての接触は
					// 異なるContactとなる。
					// それを剛体ごとにまとめるには皮膚感覚の加算についての知見が必要。
					// とりあえずあとまわし (mitake, 09/02/07)

					shapePair = solidPair->GetShapePair(s, t);

					if (!shapePair) { continue; }

					int			contactStat	= solidPair->GetContactState(s, t);
					unsigned	lastContCnt	= solidPair->GetLastContactCount(s, t);

					if (contactStat == 1 || (contactStat == 2 && lastContCnt == sceneCnt)) {
						totalForce += force;

						double		depth			= solidPair->GetContactDepth(s, t);
						int			nSectionVtx		= shapePair->NSectionVertexes();

						CRContactInfo contact;
						contact.pos		= solidPair->GetCommonPoint(s, t);
						contact.soMe	= soMe;
						contact.soOther	= soOther;
						contact.force	= force;
						if (nSectionVtx != 0) {
							contact.dimension	= shapePair->GetContactDimension();
							contact.normal		= shapePair->GetNormalVector();
							contact.pressure	= fabs(force.dot(contact.normal));
						} else {
							// 接触面の曲率半径
							CDConvexIf* sh1 = shapePair->GetShape(0)->Cast();
							Quaterniond q1 = (so1->GetShapePose(0) * so1->GetPose()).Inv();
							double r1 = sh1->CurvatureRadius(q1 * contact.pos);

							CDConvexIf* sh2 = shapePair->GetShape(1)->Cast();
							Quaterniond q2 = (so2->GetShapePose(1) * so2->GetPose()).Inv();
							double r2 = sh2->CurvatureRadius(q2 * contact.pos);

							// Hertzの接触公式
							double E = 4.2e+5;	/// -- 皮膚のYoung率   この辺に実の値を使うのはいろいろ無意味かも。
							double v = 0.49;	/// -- 皮膚のPoisson比
							double P = force.norm();  /// -- 押し込み力
							double a = pow((3.0/4.0)*P*( 2*(1-v*v)/E )/(1/r1 + 1/r2), 1/3.0);
							double pMax = 3*P/(2*3.1415926*a*a);

							contact.dimension	= 1e-4;
							contact.pressure	= pMax;
						}
						contactList.push_back(contact);
					}

				}
			}

		}
	}
}

} // namespace Spr
