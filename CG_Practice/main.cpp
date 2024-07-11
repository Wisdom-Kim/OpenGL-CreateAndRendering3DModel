#include <GL/glut.h>
#include <iostream>
#include "showModel.h"
#include "SOR.h"

using namespace std;

int main(int argc, char** argv) {
    int choice;
    cout << "Select mode: 1 for Show 3D Model, 2 for SOR: ";
    cin >> choice;

    glutInit(&argc, argv);

    if (choice == 1) {
        ShowModel_main();
    }
    else if (choice == 2) {
        SOR_main();
    }
    else {
        cout << "Invalid choice!" << endl;
        return 1;
    }

    glutMainLoop();
    return 0;
}
