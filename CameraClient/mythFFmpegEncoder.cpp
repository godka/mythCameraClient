#include "mythFFmpegEncoder.hh"
#include <stdio.h>
mythFFmpegEncoder::mythFFmpegEncoder(void* phwnd, int width, int height)
{
	mwidth = width;
	mheight = height;
	video_codec = NULL;
	this->c = NULL;
	this->frame = NULL;
	hwnd = phwnd;
	Init();
}

void mythFFmpegEncoder::yuv2RGB(int width, int height,
	const void* ysrc, const void* usrc, const void* vsrc,
	int ysize, int usize, int vsize, void** dst){
	const void* src [] = { ysrc, vsrc, usrc };
	int src_linesize [] = { ysize, vsize, usize };
	yuv2RGB(width, height, (const void**) &src, src_linesize, dst);
	return;
}

void mythFFmpegEncoder::RGB2yuv(int width, int height, const void* src, void** dst){
	struct SwsContext *img_convert_ctx = sws_getContext(
		width, height, PIX_FMT_RGB24,
		width, height, PIX_FMT_YUV420P,
		SWS_FAST_BILINEAR, NULL, NULL, NULL);
	uint8_t *rgb_src[3] = { (uint8_t *) src, NULL, NULL };
	int srcwidth [] = { width * 3, 0, 0 };
	int dstwidth [] = { width, width / 2, width / 2 };
	if (img_convert_ctx){
		sws_scale(img_convert_ctx, (const uint8_t *const*) rgb_src, srcwidth, 0, height,
			(uint8_t *const*) dst, dstwidth);
	}
	sws_freeContext(img_convert_ctx);
	return;
}
void mythFFmpegEncoder::yuv2RGB(int width, int height, const void** src, int* src_linesize, void** dst){
	struct SwsContext *img_convert_ctx = sws_getContext(
		width, height, PIX_FMT_YUV420P,
		width, height, PIX_FMT_RGBA,
		SWS_FAST_BILINEAR, NULL, NULL, NULL);
	int dstwidth [] = { width * 4, width * 4, width * 4 };
	if (img_convert_ctx){
		sws_scale(img_convert_ctx, (const uint8_t *const*) src, src_linesize, 0, height,
			(uint8_t *const*) dst, dstwidth);
	}
	sws_freeContext(img_convert_ctx);
	return;
}
mythFFmpegEncoder* mythFFmpegEncoder::CreateNew(void* phwnd, int width, int height)
{
	return new mythFFmpegEncoder(phwnd, width, height);
}
bool mythFFmpegEncoder::Init(){
	//if it has been initialized before, we should do cleanup first
	Cleanup();
	/* Initialize libavcodec, and register all codecs and formats. */
	av_register_all();
	avcodec_register_all();

	video_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!video_codec){
		Cleanup();
		return false;
	}
	c = avcodec_alloc_context3(video_codec);
	AVDictionary *opts = NULL;
	av_dict_set(&opts, "b", "2.5M", 0);
	c->width = mwidth;
	c->height = mheight;
	c->bit_rate = 100000;
	c->gop_size = 25;
	AVRational ration = { 1, 25 };
	c->time_base = ration;
	c->pix_fmt = PIX_FMT_YUV420P;
	c->max_b_frames = 1;

	av_opt_set(c->priv_data, "preset", "ultrafast", 0);   //ultrafast,superfast, veryfast, faster, fast, medium, slow, slower, veryslow,placebo.
	//av_opt_set(c->priv_data, "profile", "high", 0);        //baseline main high
	//av_opt_set(c->priv_data, "level", "4.0", 0);
	av_opt_set(c->priv_data, "tune", "zerolatency", 0); //  tune

	/* open the codec */
	int ret = avcodec_open2(c, video_codec, &opts);
	if (ret < 0) {
		Cleanup();
		return false;
	}

	/* allocate and init a re-usable frame */
	frame = avcodec_alloc_frame();
	if (!frame) {
		Cleanup();
		return false;
	}
	frame->format = c->pix_fmt;
	frame->width = c->width;
	frame->height = c->height;
	av_init_packet(&avpkt);
	return true;
}
mythFFmpegEncoder::~mythFFmpegEncoder(void)
{
	Cleanup();
}
void mythFFmpegEncoder::Cleanup(){
	//if (avpkt != NULL){
	//		av_free_packet(&avpkt);
	//	}//avpkt = NULL;
	if (this->frame != NULL){
		av_free(frame);
		frame = NULL;
	}
	if (this->c != NULL){
		avcodec_close(c);
		av_free(c);
		c = NULL;
	}
}

void mythFFmpegEncoder::ProcessFrame(unsigned char** src, int* srclinesize, responseHandler* response){

	avpkt.data = NULL;
	avpkt.size = 0;
	for (int i = 0; i < 3; i++){
		frame->data[i] = src[i];
		frame->linesize[i] = srclinesize[i];
	}
	if (c == NULL && frame == NULL)return;
	//while (avpkt.size > 0) {
	int got_frame = 0;
	int len = avcodec_encode_video2(c, &avpkt, frame, &got_frame);
	if (len < 0) {
		return;
	}
	if (got_frame){
		//callback
		response(this->hwnd, (char*) avpkt.data, avpkt.size);
	}
	else{
		printf("encode failed\n");
	}
	//}
}
