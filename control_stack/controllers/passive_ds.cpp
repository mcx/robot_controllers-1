#include "passive_ds.hpp"
#include <assert.h>

namespace control_stack
{
    namespace controllers
    {
        void PassiveDS::Orthonormalize()
        {
            assert(state_.basis_matrix_.rows() == state_.basis_matrix_.cols());
            unsigned int dim = state_.basis_matrix_.rows();
            state_.basis_matrix_.col(0).normalize();
            for(unsigned int i=1;i<dim;i++){
                for(unsigned int j=0;j<i;j++)
                    state_.basis_matrix_.col(i) -= state_.basis_matrix_.col(j).dot(state_.basis_matrix_.col(i))*state_.basis_matrix_.col(j);
                state_.basis_matrix_.col(i).normalize();
            }
        }

        void PassiveDS::AssertOrthonormalize()
        {
            uint dim = state_.basis_matrix_.cols();
            for (int i = 0; i < dim; ++i) {
                assert(abs(state_.basis_matrix_.col(i).norm()-1.0) < FLOATEQUAL);
                for (int j = 0; j < i; ++j) {
                    assert(abs(state_.basis_matrix_.col(i).dot(state_.basis_matrix_.col(j)))<FLOATEQUAL);
                }

            }
        }

        void PassiveDS::ComputeOrthonormalBasis()
        {
            auto dir = q_.desired_velocity_.normalized(); // or normilize
            assert(dir.rows()==state_.basis_matrix_.rows());
            state_.basis_matrix_.col(0)=dir;
            Orthonormalize();
        }

        void PassiveDS::ComputeDamping()
        {
                // only proceed of we have a minimum velocity norm!
            if(q_.desired_velocity_.norm() > MINSPEED)
                ComputeOrthonormalBasis();
            // otherwise just use the last computed basis
            state_.damping_matrix_ = state_.basis_matrix_*state_.eig_matrix_*state_.basis_matrix_.transpose();

        }

        void PassiveDS::Update()
        {
            ComputeDamping();
            u_.effort_ =  -state_.damping_matrix_*q_.current_velocity_ + state_.eig_matrix_(0,0)*q_.desired_velocity_;
        }

    } // namespace controller

} // namespace control_stack