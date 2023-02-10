#include "renderer.h"
static vec_t sunpos;

vec_t hash(vec_t p){// ハッシュ生成
	vec_t p3=vfract(vmul(p,vec(.1031,.1030,.0973)));
	p3=fadd(p3,dot(p3,fadd(vec(p3.y,p3.z,p3.x),33.33)));
	return vfract(vec((p3.x+p3.y)*p3.z,(p3.x+p3.z)*p3.y,(p3.y+p3.z)*p3.x));
}
float noise(vec_t p){// パーリンノイズ
	vec_t
		i=vfloor(p),// 整数部
		f=vfract(p),// 小数部
		u=vmul(vmul(vmul(f,f),f),fadd(vmul(f,fadd(fmul(f,6.),-15.)),10.));// 補間用の数
	return mix(// 格子点の数を内積を使って滑らかに補間する
		mix(dot(hash(i),f),
				dot(hash(vadd(i,vec(1.,0.,0.))),vsub(f,vec(1.,0.,0.))),u.x),
		mix(dot(hash(vadd(i,vec(0.,1.,0.))),vsub(f,vec(0.,1.,0.))),
				dot(hash(vadd(i,vec(1.,1.,0.))),vsub(f,vec(1.,1.,0.))),u.x),u.y);
}
float fBM(vec_t p){// フラクタルブラウニアンモーション = ノイズの合成
	vec_t p_=fadd(p,1024.);// アーティファクト防止
	float x=0.,a=1.,m=0.;// 結果 倍率 値域
	for(uint8_t i=0;i<4;i++){x+=a*noise(p_);m+=a;p_=fmul(p_,4.);a*=.2;}// 変化させながら重ね合わせる
	return smoothstep(-m,m,x)*2.-1.;// 値域で滑らかにして -1 ~ 1 で正規化
}

float dfx(vec_t p){// 距離関数
	return fmin(
		p.y-fBM(fmul(vec(p.x,p.z,0.),.3))*2.5,// 地形
		p.y// 水面
	);
}
vec_t nfx(vec_t p){// 法線関数
	float
		d=.01,// 微小量の定義
		dfxp=dfx(p);// 基準座標の距離
	return normalize(vec(// 基準から少しずれた座標の距離との差をとる
		dfx(vadd(p,vec(  d,0.,0.)))-dfxp,
		dfx(vadd(p,vec(0.,  d,0.)))-dfxp,
		dfx(vadd(p,vec(0.,0.,  d)))-dfxp 
	));
}

float getlum(vec_t rPos){return clamp(dot(nfx(rPos),sunpos)*.8+.2,.2,1.);}// 明るさを法線と光線ベクトルの内積から取る
float getpat(vec_t rPos){return smoothstep(.3,.7,noise(fmul(vec(rPos.x,rPos.z,rPos.y),4.)))*.2+.8;}// ノイズで模様を作成

#define CLOUD else if(rPos.y>4.){c[x]=vmix(c[x],vec(.9,.9,.9),smoothstep(.5,.8,noise(fmul(vec(rPos.x,rPos.z,rPos.y),.2))*smoothstep(4,16,length(rPos))));break;}// 雲と空の色をノイズと距離で混ぜる


FILE *render(FILE *f,uint16_t w,uint16_t h,float t){// 描画関数 返値をFILE*にすることでmain側でFILE*の変数を定義せずに済む
	vec_t c[w],p,rPos,cPos,cDir,cSide,ray;// 変数群の定義
	float dist,tmp;
	sunpos=normalize(vec(1.,1.,1.));// 太陽
	printf("...\n");// 進捗表示初期化
	for(uint16_t y=0;y<h;y++){
		printf("\033[F\033[2K\033[G  y: %3d / %3d\n",y,h);// 進捗いかが?
		for(uint16_t x=0;x<w;x++){
			p=fdiv(vsub(fmul(vec(x,h-y,0.),2.),vec(w,h,0.)),fmin(w,h));// 正規化した画面座標

			cPos=vec(4.*sin(M_PI*t*2.),1.,4.*cos(M_PI*t*2.));// カメラ座標
			cDir=normalize(vsub(// カメラ向き
				vec(5.*sin(M_PI*(t+.2)*2.),.5,5.*cos(M_PI*(t+.2)*2.)),// カメラ注視点
			cPos));
			cSide=normalize(cross(cDir,vec(0.,1.,0.)));// カメラ横 一度上方向をを0,1,0と置いて作成 本当の上方向は後でcrossで出す
			ray=normalize(vadd(vadd(fmul(cSide,p.x),fmul(cross(cSide,cDir),p.y)),fmul(cDir,1.)));// 視線の定義

			c[x]=vmix(vec(.75,.81,.94),vec(.66,.80,.98),smoothstep(-.1,.3,fabs(ray.y)));// 空の色 地平線付近で滑らかに変える 水面反射の場合分けが面倒なので絶対値をとる

			dist=0.;// 被写体の距離
			rPos=cPos;// 視線座標
			for(uint8_t i=0;i<48;i++){// レイマーチング
				dist=dfx(rPos);// 距離の取得
				if(dist<0.){// 衝突判定 前回よりも緩い条件にすることで穴を消した
					if(rPos.y<0.){// 水面
						tmp=getpat(rPos);// 先に水面の座標からノイズで模様を作成
						ray.y=-ray.y;// y軸反転
						rPos.y=0.;// 埋もれた視線を水面に戻す

						for(uint8_t j=0;j<24;j++){// 軽くレイマーチング
							dist=dfx(rPos);
							if(dist<0.){
								c[x]=vmix(fmul(vec(.34,.77,0.),getlum(rPos)),c[x],smoothstep(4,12,length(rPos)));// 反射した地面の描画 ノイズは省略
								break;
							}
							CLOUD// 雲の描画
							rPos=vadd(rPos,fmul(ray,fmax(dist,.2)));// 視線の座標の更新 解像度は落ちていいので雲まで視線を届けたい
						}

						c[x]=vmix(c[x],fmul(vec(.11,.63,.95),tmp),ray.y);// 水面の座標で模様を付けた水の色と反射を視線ベクトルのsinでそれっぽく混ぜる
					}else{// 地面
						c[x]=vmix(fmul(// 色と明るさの乗算
							fmul(vec(.34,.77,0.),getpat(rPos)),// 地面の色*ノイズ
							getlum(rPos)// 明るさ
						),c[x],smoothstep(4,12,length(rPos)));// 距離で霧をかけて描画の橋を誤魔化す
					}
					break;
				}
				CLOUD// 雲の描画
				rPos=vadd(rPos,fmul(ray,fmax(dist,.04)));// 視線の座標の更新 視線が山の尾根で引っかかりやすいので、短い場合は強制的に進めて解決
			}
			c[x]=vmix(c[x],vec(1.,1.,.9),smoothstep(.95,1.,dot(ray,sunpos)));// 視線ベクトルの向きから太陽を描画 水面にも映る
		}
		fpush(f,c,w);
	}
	printf("\033[F\033[2K\033[G");// 進捗表示終了
	return f;
}