/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.1
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Application
    parallelLoopTest

Description
    

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "simpleControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

label innerFunc(const fvMesh& mesh)
{
    // Dummy operations
    (void) Foam::pow(mesh.C()[0], 2);
    return 1;
}

void outerFunc(const fvMesh& mesh, const label& nCells)
{
    // Does some calculations until stop reaches global nCells
    label stop = 0;
    do
    {
        // Perform calculations on all processor
        stop += innerFunc(mesh);

        // On some condition, a processor should not continue, and immediately
        // return to main
        if (Pstream::myProcNo() == 1)
        {
            return;
        }

        // ??? who's going to reduce this?
        reduce(stop, sumOp<label>());
    } while (stop < nCells);
}


int main(int argc, char *argv[])
{
#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    // Global number of mesh cells
    label nCells = mesh.nCells();
    reduce(nCells, sumOp<label>());

    outerFunc(mesh, nCells);

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
