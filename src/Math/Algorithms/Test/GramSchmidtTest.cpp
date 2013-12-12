/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <TestSuite/Tester.h>

#include "Math/Algorithms/GramSchmidt.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test {

class GramSchmidtTest: public Corrade::TestSuite::Tester {
    public:
        GramSchmidtTest();

        void orthogonalize();
        void orthonormalize();
};

typedef RectangularMatrix<3, 3, Float> Matrix3x3;
typedef Vector<3, Float> Vector3;

GramSchmidtTest::GramSchmidtTest() {
    addTests<GramSchmidtTest>({&GramSchmidtTest::orthogonalize,
              &GramSchmidtTest::orthonormalize});
}

void GramSchmidtTest::orthogonalize() {
    Matrix3x3 m(Vector3(3.0f,  5.0f, 1.0f),
                Vector3(4.0f,  4.0f, 7.0f),
                Vector3(7.0f, -1.0f, -3.0f));

    Matrix3x3 orthogonalized = Algorithms::gramSchmidtOrthogonalize(m);

    /* Verify the first vector is in direction of first original */
    CORRADE_COMPARE(orthogonalized[0], m[0]);

    /* (The vectors don't need to unit length) */

    /* Verify the vectors are orthogonal */
    CORRADE_COMPARE(Vector3::dot(orthogonalized[0], orthogonalized[1]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(orthogonalized[0], orthogonalized[2]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(orthogonalized[1], orthogonalized[2]), 0.0f);

    /* Just to be sure */
    Matrix3x3 expected(Vector3(     3.0f,       5.0f,       1.0f),
                       Vector3(0.657143f, -1.571429f,  5.885714f),
                       Vector3(6.086759f,   -3.3379f, -1.570777f));
    CORRADE_COMPARE(orthogonalized, expected);
}

void GramSchmidtTest::orthonormalize() {
    Matrix3x3 m(Vector3(3.0f,  5.0f, 8.0f),
                Vector3(4.0f,  4.0f, 7.0f),
                Vector3(7.0f, -1.0f, 8.0f));

    Matrix3x3 orthonormalized = Algorithms::gramSchmidtOrthonormalize(m);

    /* Verify the first vector is in direction of first original */
    CORRADE_COMPARE(orthonormalized[0], m[0].normalized());

    /* Verify the vectors have unit length */
    CORRADE_COMPARE(orthonormalized[0].length(), 1.0f);
    CORRADE_COMPARE(orthonormalized[1].length(), 1.0f);
    CORRADE_COMPARE(orthonormalized[2].length(), 1.0f);

    /* Verify the vectors are orthogonal */
    CORRADE_COMPARE(Vector3::dot(orthonormalized[0], orthonormalized[1]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(orthonormalized[0], orthonormalized[2]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(orthonormalized[1], orthonormalized[2]), 0.0f);

    /* Just to be sure */
    Matrix3x3 expected(Vector3( 0.303046f,  0.505076f,  0.808122f),
                       Vector3( 0.928316f, -0.348119f, -0.130544f),
                       Vector3(-0.215388f, -0.789754f,  0.574367f));
    CORRADE_COMPARE(orthonormalized, expected);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::GramSchmidtTest)
