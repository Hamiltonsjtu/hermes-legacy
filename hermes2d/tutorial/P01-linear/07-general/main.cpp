#define HERMES_REPORT_ALL
#define HERMES_REPORT_FILE "application.log"
#include "hermes2d.h"

//  This example solves a general second-order linear equation with non-constant
//  coefficients, and shows how integration orders in linear and bilinear forms
//  can be defined manually.
//
//  PDE: -d/dx(a_11(x,y)du/dx) - d/dx(a_12(x,y)du/dy) - d/dy(a_21(x,y)du/dx) - d/dy(a_22(x,y)du/dy)
//       + a_1(x,y)du/dx + a_2(x,y)du/dy + a_0(x,y)u = rhs(x,y)
//
//  Domain: arbitrary
//
//  BC:  Dirichlet for boundary marker 1: u = g_D(x,y)
//       Natural for any other boundary marker:   (a_11(x,y)*nu_1 + a_21(x,y)*nu_2) * dudx
//                                              + (a_12(x,y)*nu_1 + a_22(x,y)*nu_2) * dudy = g_N(x,y)
//
//  The following parameters can be changed:

const int P_INIT = 2;                             // Initial polynomial degree of all mesh elements.
const int INIT_REF_NUM = 3;                       // Number of initial uniform refinements.
MatrixSolverType matrix_solver = SOLVER_UMFPACK;  // Possibilities: SOLVER_AMESOS, SOLVER_AZTECOO, SOLVER_MUMPS,
                                                  // SOLVER_PETSC, SOLVER_SUPERLU, SOLVER_UMFPACK.

const char* iterative_method = "cg";              // Name of the iterative method employed by AztecOO (ignored
                                                  // by the other solvers).
                                                  // Possibilities: gmres, cg, cgs, tfqmr, bicgstab.
const char* preconditioner = "jacobi";            // Name of the preconditioner employed by AztecOO (ignored by
                                                  // the other solvers).
                                                  // Possibilities: none, jacobi, neumann, least-squares, or a
                                                  // preconditioner from IFPACK (see solver/aztecoo.h).

// Boundary markers.
const int BDY_HORIZONTAL = 1, BDY_VERTICAL = 2;

// Weak forms.
#include "forms.cpp"

int main(int argc, char* argv[])
{
  // Time measurement.
  TimePeriod cpu_time;
  cpu_time.tick();

  // Load the mesh.
  Mesh mesh;
  H2DReader mloader;
  mloader.load("domain.mesh", &mesh);

  // Perform initial mesh refinements.
  for (int i=0; i < INIT_REF_NUM; i++) mesh.refine_all_elements();

  // Initialize boundary conditions
  WeakFormTutorial::DirichletFunctionBoundaryConditionTutorial *bc1
      = new WeakFormTutorial::DirichletFunctionBoundaryConditionTutorial(Hermes::vector<int>(BDY_HORIZONTAL));
  NeumannValueBoundaryCondition *bc2 = new NeumannValueBoundaryCondition(Hermes::vector<int>(BDY_VERTICAL), 0.0);
  BoundaryConditions *bcs = new BoundaryConditions(Hermes::vector<BoundaryCondition *>(bc1, bc2));

  // doesn't work and I don't know why
  // DirichletFunctionBoundaryConditionTutorial bc1(Hermes::vector<int>(BDY_HORIZONTAL));
  // NeumannValueBoundaryCondition bc2(Hermes::vector<int>(BDY_VERTICAL), 0.0);
  // BoundaryConditions *bcs = new BoundaryConditions(Hermes::vector<BoundaryCondition *>(&bc1, &bc2));

  // Create an H1 space with default shapeset.
  H1Space space(&mesh, bcs, P_INIT);
  int ndof = Space::get_num_dofs(&space);
  info("ndof = %d", ndof);

  // Initialize the weak formulation.
  WeakFormTutorial wf;

  // Initialize the FE problem.
  bool is_linear = true;
  DiscreteProblem dp(&wf, &space, is_linear);

  SparseMatrix* matrix = create_matrix(matrix_solver);
  Vector* rhs = create_vector(matrix_solver);
  Solver* solver = create_linear_solver(matrix_solver, matrix, rhs);

  if (matrix_solver == SOLVER_AZTECOO)
  {
    ((AztecOOSolver*) solver)->set_solver(iterative_method);
    ((AztecOOSolver*) solver)->set_precond(preconditioner);
    // Using default iteration parameters (see solver/aztecoo.h).
  }

  // Initialize the solution.
  Solution sln;

  // Assemble the stiffness matrix and right-hand side vector.
  info("Assembling the stiffness matrix and right-hand side vector.");
  dp.assemble(matrix, rhs);

  // Solve the linear system and if successful, obtain the solution.
  info("Solving the matrix problem.");
  if(solver->solve())
    Solution::vector_to_solution(solver->get_solution(), &space, &sln);
  else
    error ("Matrix solver failed.\n");

  // Time measurement.
  cpu_time.tick();

  // Clean up.
  delete solver;
  delete matrix;
  delete rhs;

  // View the solution and mesh.
  ScalarView sview("Solution", new WinGeom(0, 0, 440, 350));
  sview.show(&sln);
  OrderView  oview("Polynomial orders", new WinGeom(450, 0, 400, 350));
  oview.show(&space);

  // Skip visualization time.
  cpu_time.tick(HERMES_SKIP);

  // Print timing information.
  verbose("Total running time: %g s", cpu_time.accumulated());

  // Wait for all views to be closed.
  View::wait();

  return 0;
}
