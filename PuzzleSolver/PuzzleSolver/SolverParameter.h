//当たり判定や評価関数で用いる、パラメータを定義します。
//許容誤差変数, 重み変数, を構造体で管理します

#pragma once

class SolverParameter {
public:
	double dist_error_waku_meter;	//当たり判定：枠との当たり判定は緩めにしたい
	double dist_error_meter;		//当たり判定, 評価関数, マージ処理：きつくするとパズルが完成しないし、緩いと変なのが入る
	double pixel_per_meter;			//1メートルあたりのピクセル数

	double dist_error_waku() {
		return dist_error_waku_meter * pixel_per_meter;
	}

	double dist_error() {
		return dist_error_meter * pixel_per_meter;
	}

	//角度の許容誤差を°で返す
	double angle_error(double line_length) {
		//tan(angle_error) <= dist_error / line_length
		double angle_error = asin(dist_error() / line_length) * 180 / 3.1415926;
		return angle_error;
	}
};
