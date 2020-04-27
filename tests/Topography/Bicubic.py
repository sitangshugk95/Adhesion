#
# Copyright 2020 Lars Pastewka
#
# ### MIT license
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

"""
Tests for the bicubic interpolation module
"""

import pytest

import numpy as np

from PyCo.Tools.Interpolation import Bicubic

nx = 17
ny = 22

def test_grid_values(tol=1e-9):
    field = np.random.random([nx, ny])
    interp = Bicubic(field)
    for i in range(nx):
        for j in range(ny):
            assert abs(interp(i, j) - field[i, j]) < tol

    x, y = np.mgrid[:nx, :ny]

    for der in [0, 1, 2]:
        if der == 0:
            interp_field = interp(x, y, derivative=der)
        elif der == 1:
            interp_field, _, _ = interp(x, y, derivative=der)
        else:
            interp_field, _, _, _, _, _ = interp(x, y, derivative=der)
        assert np.allclose(interp_field, field)


def test_wrong_derivative(tol=1e-9):
    field = np.random.random([nx, ny])
    interp = Bicubic(field)
    with pytest.raises(ValueError):
        interp(1, 1, derivative=3)
    with pytest.raises(ValueError):
        interp(1, 1, derivative=-1)


def test_wrong_grid(tol=1e-9):
    field = np.random.random([nx, ny])
    derx = np.random.random([nx, ny-1])
    dery = np.random.random([nx, ny])
    with pytest.raises(ValueError):
        Bicubic(field, derx, dery)


def test_grid_derivatives(tol=1e-9):
    field = np.random.random([nx, ny])
    derx = np.random.random([nx, ny])
    dery = np.random.random([nx, ny])
    interp = Bicubic(field, derx, dery)
    for i in range(nx):
        for j in range(ny):
            assert abs(interp(i, j) - field[i, j]) < tol

    x, y = np.mgrid[:nx, :ny]

    interp_field, interp_derx, interp_dery = interp(x, y, derivative=1)
    assert np.allclose(interp_field, field)
    assert np.allclose(interp_derx, derx)
    assert np.allclose(interp_dery, dery)