Atamai Image Guided Surgery package (AIGS)

This is a collection of VTK classes for use with image guided surgery
applications.  The main body of this package is a collection of VTK
classes for various localizer devices:

vtkNDITracker - for the POLARIS, POLARIS SPECTRA, POLARIS VICRA, and AURORA
vtkAscension3DGTracker - for the 3DG tracking family (medSAFE, driveBAY, trakSTAR, trakSTAR 2)
vtkNDICertusTracker - for the Optotrak Certus
vtkFlockTracker - for the Flock of Birds (hasn't been updated in a while)
vtkMicronTracker - for the Claron MicronTracker
vtkLogitechTracker - for the old Logitech 3D Mouse
vtkPolhemusTracker - for the Polhemus FasTrak (not functional)

At Qt Widget and some example code have been added to this library in the
GUI and Examples folders.  They are provided as examples primarily for
NDI products.
