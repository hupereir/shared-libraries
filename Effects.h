#ifndef Effects_h
#define Effects_h

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

/*!
\file Effects.h
\brief some graphic effects
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cmath>
#include <QImage>
#include <QPainter>

namespace Transparency
{
    //! Exponential blur, Jani Huhtanen, 2006
    class Effects
    {

        public:

        //! blur image
        static void shadowBlur(QImage &image, int radius, const QColor &color)
        {
            if (radius < 1) return;
            expBlur<16, 7>(image, radius);

            QPainter p(&image);
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(image.rect(), color);
            p.end();
        }

        private:

        /*
        *  expBlur(QImage &img, int radius)
        *
        *  In-place blur of image 'img' with kernel
        *  of approximate radius 'radius'.
        *
        *  Blurs with two sided exponential impulse
        *  response.
        *
        *  aprec = precision of alpha parameter
        *  in fixed-point format 0.aprec
        *
        *  zprec = precision of state parameters
        *  zR,zG,zB and zA in fp format 8.zprec
        */
        template<int aprec,int zprec>
            static void expBlur(QImage &img, int radius)
        {
            if(radius < 1) {
                return;
            }

            /* Calculate the alpha such that 90% of
            the kernel is within the radius.
            (Kernel extends to infinity)
            */
            int alpha = (int)((1 << aprec) * (1.0f - std::exp(-2.3f / (radius + 1.f))));

            for (int row=0; row<img.height(); row++) {
                blurRow<aprec,zprec>(img, row, alpha);
            }

            for (int col=0; col<img.width(); col++) {
                blurCol<aprec,zprec>(img, col, alpha);
            }
            return;
        }

        //! inner blur
        template<int aprec, int zprec>
            static inline void blurInner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha);

        //! horizontal blur
        template<int aprec,int zprec>
            static inline void blurRow(QImage &im, int line, int alpha);

        //! vertical blur
        template<int aprec, int zprec>
            static inline void blurCol(QImage &im, int col, int alpha);

        template<class T>
            static inline const T &qClamp(const T &x, const T &low, const T &high)
        {
            if (x <  low) return low;
            else if (x > high) return high;
            else return x;
        }

    };

    //______________________________________________________________________________
    template<int aprec,int zprec>
        void Effects::blurRow(QImage &im, int line, int alpha)
    {
        int zR, zG, zB, zA;

        QRgb *ptr = (QRgb *)im.scanLine(line);

        zR = *((unsigned char *)ptr    ) << zprec;
        zG = *((unsigned char *)ptr + 1) << zprec;
        zB = *((unsigned char *)ptr + 2) << zprec;
        zA = *((unsigned char *)ptr + 3) << zprec;

        for (int index=1; index<im.width(); index++) blurInner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);
        for (int index=im.width()-2; index>=0; index--) blurInner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);

    }

    //______________________________________________________________________________
    template<int aprec, int zprec>
        void Effects::blurCol(QImage &im, int col, int alpha)
    {
        int zR, zG, zB, zA;

        QRgb *ptr = (QRgb *)im.bits();
        ptr += col;

        zR = *((unsigned char *)ptr    ) << zprec;
        zG = *((unsigned char *)ptr + 1) << zprec;
        zB = *((unsigned char *)ptr + 2) << zprec;
        zA = *((unsigned char *)ptr + 3) << zprec;

        for (int index=im.width(); index<(im.height()-1)*im.width(); index+=im.width()) blurInner<aprec,zprec>((unsigned char *)&ptr[index], zR, zG, zB, zA, alpha);
        for (int index=(im.height()-2)*im.width(); index>=0; index-=im.width()) blurInner<aprec,zprec>((unsigned char *)&ptr[index], zR, zG, zB, zA, alpha);
    }

    //______________________________________________________________________________
    template<int aprec, int zprec>
        void Effects::blurInner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha)
    {
        int R, G, B, A;
        R = *bptr;
        G = *(bptr + 1);
        B = *(bptr + 2);
        A = *(bptr + 3);

        zR += (alpha * ((R << zprec) - zR)) >> aprec;
        zG += (alpha * ((G << zprec) - zG)) >> aprec;
        zB += (alpha * ((B << zprec) - zB)) >> aprec;
        zA += (alpha * ((A << zprec) - zA)) >> aprec;

        *bptr =     zR >> zprec;
        *(bptr+1) = zG >> zprec;
        *(bptr+2) = zB >> zprec;
        *(bptr+3) = zA >> zprec;
    }

};

#endif
