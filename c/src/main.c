#include "main.h"
#include "renderer.h"

FILE *finit(uint16_t n,uint16_t w,uint16_t h){// ppm準備
	char fn[16];// ファイル名一時保存
	sprintf(fn,"img/%05d.ppm",n);// ファイル名作成
	FILE *f=fopen(fn,"wb");// ファイル取得
	if(f==NULL){fprintf(stderr,"can't open %s\n",fn);exit(1);}// ファイルエラー処理
	fprintf(f,"P6\n%d %d\n255\n",w,h);// ヘッダー書き込み
	return f;
}
void fpush(FILE *f,vec_t c[],uint16_t l){// ppm書き足し
	uint8_t buf[3*l];// 変換データ一時保存 可変長配列 uint8_tなので問題なし
	for(uint16_t i=0;i<l;i++){// floatからuint8_tに変換
		buf[i*3+0]=c[i].x*255;
		buf[i*3+1]=c[i].y*255;
		buf[i*3+2]=c[i].z*255;
	}
	fwrite(buf,3*l,1,f);// データ書き込み
}

int main(int argc,char *argv[]){
	if(argc!=4)printf("uint16_t argv[3]=[ width, height, frame_count ]\n");
	else{
		printf("rendering...\n...\n");// 進捗表示初期化
		uint16_t
			w=strtoul(argv[1],NULL,10),// 幅
			h=strtoul(argv[2],NULL,10),// 高さ
			fc=strtoul(argv[3],NULL,10);// フレーム数
		for(uint16_t i=0;i<fc;i++){
			printf("\033[F\033[2K\033[Gimg: %3d / %3d\n",i,fc);// 進捗いかが?
			fclose(render(finit(i,w,h),w,h,(float)i/fc));// ppm準備->描画->ppm終了
		}
		printf("\033[F\033[2K\033[Gdone!\n");// 進捗表示終了 完了表示
	}
	return EXIT_SUCCESS;
}