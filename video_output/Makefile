FRAME_RATE?=200
PREFIX?=video_

video:
	ffmpeg -y -f image2 -r ${FRAME_RATE} -i ${PREFIX}%010d.png -vcodec libx264 -pix_fmt yuv420p ${PREFIX}video.mp4

play:
	mplayer video.mp4 -fs -loop 0

clean:
	rm -Rf video*
	rm -Rf *.png
	rm -Rf *.eps
	rm -Rf *.mp4
