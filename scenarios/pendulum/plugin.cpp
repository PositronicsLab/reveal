#define ODEINT_V1                  // odeint versioning
#define ODEINT_RUNGEKUTTA_STEPPER  // which stepper to use

//-----------------------------------------------------------------------------

#include "pendulum.h"
#include <Reveal/analytics/types.h>
#include <Reveal/pointers.h>
#include <Reveal/solution_set.h>

//-----------------------------------------------------------------------------

using namespace Reveal::Analytics;

extern "C" {

error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output ) {

  //pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );

  printf( "Analyzed pendulum\n" );

  input->scenario->print();
  printf( "\n" );

  for( unsigned i = 0; i < input->trials.size(); i++ ) {
    Reveal::Core::trial_ptr trial = input->trials[i];
    printf( "trial [%u]: ", trial->trial_id );
    trial->print();
    printf( "\n" );

    Reveal::Core::solution_ptr solution = input->solutions[i];
    printf( "solution [%u]: ", solution->trial_id );
    solution->print();
    printf( "\n" );

    Reveal::Core::solution_ptr model = input->models[i];
    printf( "model [%u]: ", model->trial_id );
    model->print();
    printf( "\n\n" );
  }

  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

//Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output );

} // extern "C"

