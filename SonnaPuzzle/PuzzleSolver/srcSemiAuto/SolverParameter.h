//当たり判定や評価関数で用いる、パラメータを定義します。
//許容誤差変数, 重み変数, を構造体で管理します

#pragma once

class SolverParameter {
public:
	double dist_error_waku;		//当たり判定：枠との当たり判定は緩めにしたい
	double dist_error;			//当たり判定, 評価関数, マージ処理：きつくするとパズルが完成しないし、緩いと変なのが入る
	double angle_error;			//評価関数, マージ処理：角度和が360 - angle_error ～ 360 + angle_errorの間なら加点
	double dist_error_tigit;	//評価関数：より正確に一致していたら高評価（ただし一致辺の評価のみに用いる）
	double angle_error_tigit;	//評価関数：より正確に一致していたら高評価

	double weight_sameAngle;		//2角が一致していたときの加点
	double weight_sameLine;			//2辺が一致していたときの加点
	double weight_straight;			//2辺が一直線上にあるときの加点
	double weight_sameHalfLine;		//2辺が同じ向きで一点で接していたときの加点

	double weight_sameAngle_tigit;	//2角が超一致していたときの加点
	double weight_sameLine_tigit;	//2辺が超一致していたときの加点
};
