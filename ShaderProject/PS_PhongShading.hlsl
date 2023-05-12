struct PS_IN
{
	float4 pos : SV_POSITION0;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL0; // モデルの法線
};

// ライトの情報
cbuffer Light : register(b0)
{
	float4 lightColor; // 光の色
	float4 lightDir; // 光の向き
};

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// 光の向きを表すベクトルと、モデルの法線ベクトルの二つが表す
	// 角度に関して、同じ向きなら影になる。反対方向だと明るくなる
	// ことが分かる。

	// そのまま計算すると、光が当たる面に影が出来てしまうので、
	// あえて光の向きを反転させる。

	// 二つのベクトルの内積から同じ向きか、反対向きか調べる。
	// ※同じ向きなら1(正の値)、反対向きなら-1(負の値)
	// 色は白が1、黒が0になるので、内積の結果をそのまま明るさとして使う


	return color;
}