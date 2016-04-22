void
convert( IplImage *src, IplImage **dst )
{
        cvCvtColor( src, grayImage, CV_BGR2GRAY );
        cvLaplace( grayImage, laplaceImage16, 3 );
        cvConvertScaleAbs( laplaceImage16, laplaceImage8, 1, 0 );

        // cvWarpAffine( laplaceImage8, dst, rotationMatrix, CV_WARP_INVERSE_MAP, cvScalarAll(0) );
        if ( src->origin == IPL_ORIGIN_TL )
                *dst = laplaceImage8;
        else
                cvFlip( laplaceImage8, *dst, 0);
}
