require 'matrix'# Vectorクラスを使いたい
$res=Vector[256.0,128.0];# 生成する画像のサイズ
$col=Array.new($res[1]){|y|Array.new($res[0]){|x|Vector[1.0,0.0,1.0,1.0];}}# 画像データ マゼンタで初期化

def mul(a,b)return a.map2(b){|x,y|x*y};end# ベクトルの要素同士の席
def mix(x,y,a)return x*(1.0-a)+y*a;end# 線形補間
def smoothstep(a,b,x)t=((x-a)/(b-a)).clamp(0.0,1.0);return t*t*(3.0-2.0*t);end# エルミート補間

def hash22(p)# ハッシュ生成 vec2 -> vec2
	p3=Vector[p[0]*0.1031,p[1]*0.1030,p[0]*0.0973].map{|x|x%1.0};
	dot=p3.dot(Vector[p3[1],p3[2],p3[0]].map{|x|x+33.33});
	p3.map!{|x|x+dot};
	return mul(Vector[p3[0]+p3[1],p3[0]+p3[2]],Vector[p3[2],p3[1]]).map{|x|x%1.0};
end
def noise(p)# パーリンノイズ
	fl=p.map{|x|x.floor};# 整数部
	fr=p.map{|x|x%1.0};# 小数部
	sfr=mul(mul(mul(fr,fr),fr),mul(fr,fr*6.0-Vector[15.0,15.0])+Vector[10.0,10.0]);# 補間用の数
	return mix(# 格子点の数を内積を使って滑らかに補間する
		mix(hash22(fl+Vector[0.0,0.0]).dot(fr-Vector[0.0,0.0]),
				hash22(fl+Vector[1.0,0.0]).dot(fr-Vector[1.0,0.0]),sfr[0]),
		mix(hash22(fl+Vector[0.0,1.0]).dot(fr-Vector[0.0,1.0]),
				hash22(fl+Vector[1.0,1.0]).dot(fr-Vector[1.0,1.0]),sfr[0]),sfr[1]);
end
def fBM(p)# フラクタルブラウニアンモーション = ノイズの合成
	p.map!{|x|x+1024.0};# アーティファクト防止
	x=0.0;a=0.5;m=0.0;# 結果 愛率 値域
	4.times{x+=a*noise(p);m+=a;p*=2.0;a*=0.5;}
	return smoothstep(-m,m,x)*2.0-1.0;# 値域で滑らかにして -1 ~ 1 で正規化
end

def dfx(p)# 距離関数
	return[
		p[1]+fBM(Vector[p[0],p[2]]*0.5)*2.0,# 地形
		p[1]# 水面
	].min;
end
def nfx(p)# 法線関数
	d=0.0001;# 微小量の定義
	dfxp=dfx(p);# 基準座標の距離
	return Vector[# 基準から少しずれた座標の距離との差をとる
		dfx(p+Vector[  d,0.0,0.0])-dfxp,
		dfx(p+Vector[0.0,  d,0.0])-dfxp,
		dfx(p+Vector[0.0,0.0,  d])-dfxp 
	].normalize;
end

def main()
	$res[1].to_i.times{|y|
		$res[0].to_i.times{|x|
			p=(Vector[x,$res[1]-y]*2.0-$res)/$res.min;# 正規化した画面座標

			fov=90.0 *Math::PI/360.0;# 視野角
			cPos=Vector[0.0,3.0,3.0];# カメラ座標
			ray=Vector[Math.sin(fov)*p[0],Math.sin(fov)*p[1],-Math.cos(fov)].normalize;# 視線の定義 上がy 奥が-z
			# $col[y][x]=Vector[ray[0],ray[1],-ray[2],1.0];# 視線を表示

			dist=0.0;# 被写体の距離
			rLen=0.0;# 視線長さ
			rPos=cPos;# 視線座標
			32.times{|i|
				dist=dfx(rPos);# 距離の取得
				rLen+=dist;# 視線の長さの更新
				rPos=cPos+ray*rLen;# 視線の座標の更新
			}
			if(dist.abs<0.001)# 衝突判定
				lum=Vector[1,1,1].dot(nfx(rPos)).clamp(0.2,1.0);# 明るさを法線と光線ベクトルの内積から取る
				col=(
					(rPos[1]<0.01)? # 水面を高度から判定
						Vector[0.11,0.63,0.95]:# 青
						Vector[0.34,0.77,0.0]# 緑
				)*((rPos[0]%0.1>0.05)^(rPos[2]%0.1>0.05)?1.0:0.8);# 市松模様
				$col[y][x]=Vector[*col,1.0]*lum;# 色と明るさを合成
			else
				$col[y][x]=Vector[0.7,0.82,0.98,1.0];# 空色
			end
			# p "#{(y+x/$res[0])/$res[1]*100} %";
		}
		p "#{y/$res[1]*100} %";# 進捗いかがですか???
	}


	name="out";# 出力ファイル名
	File.open("#{name}.ppm","wb"){|f|
		f.puts("P6\n#{$res[0].to_i} #{$res[1].to_i}\n255");# ヘッダ
		$col.each{|r|r.each{|v|
			f.write(v.to_a.slice(0,4).map{|_|(_.clamp(0.0,1.0)*v[3]*255.0).to_i}.pack("ccc"));# clampと透明度の処理と8bit化
		}}
	}
	1;
end

main();