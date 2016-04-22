void
detect_and_draw( IplImage* img )
{
        int i;

        cvCvtColor( img, gray, CV_BGR2GRAY );
        cvResize( gray, small_img, CV_INTER_LINEAR );
        cvEqualizeHist( small_img, small_img );
        cvClearMemStorage( storage );

    double t = (double)cvGetTickCount();
    CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                        1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
                                        cvSize(30, 30) );
    t = (double)cvGetTickCount() - t;
    fprintf(stderr, "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

    for( i = 0; i < (faces ? faces->total : 0); i++ ) {
        CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
        CvPoint center;
        int radius;
        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.25*scale);
        cvCircle( img, center, radius, colors[0], 3, 8, 0 );
    }

        cvShowImage( "result", img );
}
