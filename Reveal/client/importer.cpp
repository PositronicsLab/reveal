#include "Reveal/client/importer.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

importer_c::importer_c( void ) {

}
//-----------------------------------------------------------------------------
importer_c::~importer_c( void ) {

}

//-----------------------------------------------------------------------------
bool importer_c::open( void ) {
  db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );
  return db->open();
}

//-----------------------------------------------------------------------------
void importer_c::close( void ) {
  db->close();
}

//-----------------------------------------------------------------------------
bool importer_c::import_scenario( std::string id, std::string description, unsigned trials ) {

  // create and insert the scenario record
  Reveal::Core::scenario_ptr scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  scenario->id = id;
  scenario->description = description;
  scenario->trials = trials;

  // TODO : parameterize or make a later step
  scenario->steps_per_trial = 1;

  Reveal::DB::database_c::error_e error = db->insert( scenario );
  if( error == Reveal::DB::database_c::ERROR_NONE ) return true;

  return false;
}

//-----------------------------------------------------------------------------
bool importer_c::import_analyzer( std::string scenario_id, std::string analyzer_path, Reveal::Core::analyzer_c::type_e type ) {

  // create and insert the analyzer record
  Reveal::Core::analyzer_ptr analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
 
  analyzer->scenario_id = scenario_id;
  analyzer->filename = analyzer_path;
  analyzer->type = type;

  Reveal::DB::database_c::error_e error = db->insert( analyzer );
  if( error == Reveal::DB::database_c::ERROR_NONE ) return true;

  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::import_trial( std::string scenario_id, unsigned trial_id, double t, double dt, std::vector<Reveal::Core::model_ptr> models ) {
    Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
    trial->scenario_id = scenario_id;
    trial->trial_id = trial_id;
    trial->t = t;
    trial->dt = dt;

    for( unsigned i = 0; i < models.size(); i++ )
      trial->models.push_back( models[i] );

    db->insert( trial );
/*
    // record state of the system into initial trial
    trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    trial->scenario_id = scenario->id;
    trial->trial_id = trial_index++;
    trial->t = sim_time();
    trial->dt = step_size();

    // arm data
    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _arm->name();

      physics::Link_V gzlinks = _arm->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        //math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Pose pose = gzlinks[i]->GetWorldPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;        // TODO: double check ordering here
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }

      physics::Joint_V gzjoints = _arm->joints();
      for( unsigned i = 0; i < gzjoints.size(); i++ ) {
        Reveal::Core::joint_ptr joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );
        joint->id = gzjoints[i]->GetName();

        if( joint->id == _arm->shoulder_pan_actuator()->GetName() )
          joint->control[0] = sh_pan_f;
        else if( joint->id == _arm->shoulder_lift_actuator()->GetName() )
          joint->control[0] = sh_lift_f;
        else if( joint->id == _arm->elbow_actuator()->GetName() )
          joint->control[0] = elbow_f;
        else if( joint->id == _arm->wrist1_actuator()->GetName() )
          joint->control[0] = wrist1_f;
        else if( joint->id == _arm->wrist2_actuator()->GetName() )
          joint->control[0] = wrist2_f;
        else if( joint->id == _arm->wrist3_actuator()->GetName() )
          joint->control[0] = wrist3_f;
        else if( joint->id == _arm->finger_actuator_l()->GetName() )
          joint->control[0] = finger_l_f;
        else if( joint->id == _arm->finger_actuator_r()->GetName() )
          joint->control[0] = finger_r_f;
        else
          continue;    // bad joint.  Should be unreachable

        model->joints.push_back( joint );
      }

      trial->models.push_back( model );
    }

    // block data
    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _target->name();

      physics::Link_V gzlinks = _target->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        //math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Pose pose = gzlinks[i]->GetWorldPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;        // TODO : double check ordering here
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }

      trial->models.push_back( model );
    }

    db->insert( trial );
    scenario->trials = trial_index;
*/
}

//-----------------------------------------------------------------------------
bool importer_c::import_solution( std::string scenario_id, unsigned trial_id, double t, double dt, std::vector<Reveal::Core::link_ptr> links ) {
/*
    double t = sim_time();
    double dt = step_size();

    // build a model solution
    solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

    solution->scenario_id = scenario->id;
    solution->trial_id = trial->trial_id;
    solution->t = t;
    solution->dt = t;

    // arm data
    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _arm->name();

      physics::Link_V gzlinks = _arm->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        //math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Pose pose = gzlinks[i]->GetWorldPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }
      solution->models.push_back( model );
    }

    // block data
    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _target->name();

      physics::Link_V gzlinks = _target->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        //math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Pose pose = gzlinks[i]->GetWorldPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }

      solution->models.push_back( model );
    }

    db->insert( solution );
*/
}
//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

