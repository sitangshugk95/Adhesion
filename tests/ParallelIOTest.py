
import numpy as np

from PyCo.System import NonSmoothContactSystem
from PyCo.ContactMechanics import HardWall
from PyCo.SolidMechanics import FreeFFTElasticHalfSpace, PeriodicFFTElasticHalfSpace
from PyCo.Topography import make_sphere
from muFFT import NCStructuredGrid

import pytest
@pytest.mark.parametrize("HSClass", [PeriodicFFTElasticHalfSpace, FreeFFTElasticHalfSpace])
def test_NCStructuredGrid(comm, fftengine_type, HSClass):
    nx, ny = 64, 64
    sx, sy = 2., 2.

    Es=1.
    R=1.

    halfspace = HSClass((nx, ny), Es, (sx, sx),
                                        communicator=comm,
                                        fft=fftengine_type)

    topography = make_sphere(R, (nx, ny), (sx, sy),
                             nb_subdomain_grid_pts=halfspace.topography_nb_subdomain_grid_pts,
                             subdomain_locations=halfspace.topography_subdomain_locations,
                             kind="paraboloid",
                             communicator=halfspace.communicator)

    system = NonSmoothContactSystem(halfspace, HardWall(), topography)

    field_ncfile = NCStructuredGrid("field_data.nc", mode="w",
                                    nb_domain_grid_pts=system.surface.nb_grid_pts,
                                    decomposition='subdomain',
                                    subdomain_locations=system.surface.subdomain_locations,
                                    nb_subdomain_grid_pts=system.surface.nb_subdomain_grid_pts,
                                    communicator=comm)

    j=0
    for penetration in [0.1,0.5]:
        sol =system.minimize_proxy(offset=penetration)

        field_ncfile[j].penetration = penetration
        field_ncfile[j].contacting_points = np.array(sol.active_set, dtype=int)
        field_ncfile[j].gap = system.gap
        field_ncfile[j].forces = system.force

        j+=1


    field_ncfile.close()