//----------------------------------------------------------
// 概要				：エネミー2を制御するCPP
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------
//----------------------------------------------------------
//目が見えない敵
//----------------------------------------------------------


#include "Enemy/Enemy2.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sword/SwordAttackComponent.h"							//剣用

//------------------------------------------------------------------------------------------------------------
//コンストラクタ
//------------------------------------------------------------------------------------------------------------
AEnemy2::AEnemy2()
	: m_foundDistance(100.)
{
	//視界判定を使わなくする
	IsUseVisiblity = false;
	m_discoveryTime_Limit = 1.0;
}

//------------------------------------------------------------------------------------------------------------
// ゲームスタート時、または生成時に呼ばれる処理
//------------------------------------------------------------------------------------------------------------
void AEnemy2::BeginPlay()
{
	Super::BeginPlay();
}

//------------------------------------------------------------------------------------------------------------
//聴覚処理
//------------------------------------------------------------------------------------------------------------
void AEnemy2::UpdateHearing(float _deltaTime)
{

	if (m_battleCheck)
	{
		m_noiseCheck = false;		//物音チェックOF
		return;
	}

	double distance = (m_noise_Pos - m_enemyPos).Length();			//物音との距離を測る(Vectorの長さ）
	double distance_keeper = (m_noise_Pos_keeper - m_enemyPos).Length();			//以前の物音との距離を測る(Vectorの長さ）

	//物音が遠くでした場合
	if (!m_noiseCheck && distance > m_hearingRange_Long)
	{
		m_noiseCheck = false;		//物音チェックOF
		return;
	}

	//そのフレームの間にOnNoiseが呼ばれていればTrue
	if (m_isCallOnNoise)
	{
		m_isCallOnNoise_Fleam = true;
		m_isCallOnNoise = false;
	}

	//物音が出たはじめのみ
	if (!m_noiseCheck && m_noiseLevel < 6)
	{
		m_noiseLevel = 0;//物音レベルをリセット
		m_hearingTime = 0;		//聴覚時間リセット
		m_noiseCheck = true;		//物音チェックON
	}

	m_hearingTime = _deltaTime;	//聴覚時間の更新
	//OnNoiseが呼ばれていれば更新許可
	if (m_isCallOnNoise_Fleam)
	{

		//音の更新
		if (m_hearingTime < m_hearingTime_Limit)
		{
			//以前の物音が今の物音より小さい場合
			if (m_noiseVolume_keeper <= m_noiseVolume || m_noiseLevel >=6)
			{
				m_noiseVolume_keeper = m_noiseVolume;

				//距離に応じて聞こえる音レベルを変更
				if (distance < m_hearingRange_Short)
				{
					for (int i = 2; i < 6; ++i)
					{
						if (m_noiseVolume_keeper == i)
						{
							m_noiseLevel = i + 2;//物音レベルを更新
						}
					}
				}
				else if (distance < m_hearingRange_Normal)
				{
					for (int i = 2; i < 6; ++i)
					{
						if (m_noiseVolume_keeper == i)
						{
							m_noiseLevel = i + 1;//物音レベルを更新
						}
					}
				}
				else
				{
					for (int i = 2; i < 6; ++i)
					{
						if (m_noiseVolume_keeper == i)
						{
							m_noiseLevel = i;//物音レベルを更新


						}
					}
				}
				if (m_noiseLevel > 5)
				{
					m_noiseLevel = 5;
				}

				m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
				m_hearingTime = 0;	//聴覚時間リセット
			}
			//if (m_noiseLevel > 5 && m_noiseLevel != 0)
			//{
			//	m_noiseLevel = m_noiseVolume;			//物音レベルを更新
			//	m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
			//	m_hearingTime = 0;	//聴覚時間リセット
			//}

		}
		else
		{
			m_noiseVolume_keeper = m_noiseVolume;

			//距離に応じて聞こえる音レベルを変更
			if (distance < m_hearingRange_Short)
			{
				for (int i = 2; i < 6; ++i)
				{
					if (m_noiseVolume_keeper == i)
					{
						m_noiseLevel = i + 1;//物音レベルを更新
					}
				}
			}
			else if (distance < m_hearingRange_Normal)
			{
				for (int i = 2; i < 6; ++i)
				{
					if (m_noiseVolume_keeper == i)
					{
						m_noiseLevel = i;//物音レベルを更新

					}
				}
			}
			else
			{
				for (int i = 2; i < 6; ++i)
				{
					if (m_noiseVolume_keeper == i)
					{
						m_noiseLevel = i - 1;//物音レベルを更新

					}
				}
			}

			if (m_noiseLevel > 5)
			{
				m_noiseLevel = 5;
			}
			m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
			m_hearingTime = 0;	//聴覚時間リセット
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//探索処理
//------------------------------------------------------------------------------------------------------------
void AEnemy2::UpdateSearch(float _deltaTime)
{
	double distance = (m_playerPos - m_enemyPos).Length();			//プレイヤーとの距離を測る(Vectorの長さ）

	if (m_battleCheck|| distance < m_foundDistance)
	{
		m_enemyCurrentState = EEnemyStatus::Battle;
	}
	else if (m_noiseCheck)
	{
		if ((m_noiseLevel == 4 || m_noiseLevel == 5) && distance < m_hearingRange_Short && m_isCallOnNoise_Fleam == true)
		{
			m_enemyCurrentState = EEnemyStatus::Battle;
		}
		else if (m_noiseLevel <= 1)
		{
			m_enemyCurrentState = EEnemyStatus::Patrol;
		}
		else if (m_noiseLevel == 2)
		{
			m_enemyCurrentState = EEnemyStatus::Doubt_Noise;
		}
		else if (m_noiseLevel == 3)
		{
			m_enemyCurrentState = EEnemyStatus::Caution_Noise;
		}
		else if (m_noiseLevel == 4 || m_noiseLevel == 5)
		{
			m_enemyCurrentState = EEnemyStatus::Battle_Noise;
		}
	}
	//失踪の場合
	else if (m_noiseLevel == 6 || m_noiseLevel == 7)
	{
		m_enemyCurrentState = EEnemyStatus::Miss;
	}
	//帰還の場合
	else if (m_noiseLevel == 8 || m_noiseLevel == 9)
	{
		m_enemyCurrentState = EEnemyStatus::Return;
	}
	//巡回の場合
	else if (m_noiseLevel <= 1)
	{
		m_enemyCurrentState = EEnemyStatus::Patrol;
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが戦闘の場合の処理(4,5)
//------------------------------------------------------------------------------------------------------------
void AEnemy2::CaseBattle(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_battleCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Battle"));

		m_battleCheck = true;//戦闘中
		m_alertCheck = true;	//警戒状態ON
		m_currentChaseSpeed = m_chaseSpeed_Fast;							//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;

		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);

	}

	//移動する位置、視点の指定
	m_playerPos_LastSeen = m_playerPos;


	m_battleTime += _deltaTime;

	m_discoveryTime += _deltaTime;

	double distance_2D = (m_playerPos_LastSeen - m_enemyPos).Length();	//音との距離を測る(Vectorの長さ）

	double distance = (m_playerPos_LastSeen - m_enemyPos).Length();	//音との距離を測る(Vectorの長さ）

	if (m_discoveryTime < m_discoveryTime_Limit)
	{
		//視点移動処理
		UpdateViewMove(_deltaTime);
	}
	else
	{
		//プレイヤーの一定距離に近づくまで追いかける
		if (m_stopDistance_Player < distance)
		{

			//移動処理
			UpdateMove_Nav(_deltaTime);
		}

		//プレイヤーの一定距離に近くまで近づいたら
		if (m_stopDistance_Player >= distance)
		{
			if (m_sword)
			{
				//攻撃処理
				UpdateAttack(_deltaTime);
			}
		}
	}

	//もし30秒以上チェイスした場合
	if (m_battleTime > 30 || m_noiseLevel < 2)
	{
		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_noiseLevel = 6;			//見失うに移行
		m_battleCheck = false;		//戦闘終了
		m_battleTime = 0;
		m_discoveryTime = 0;
	}
}