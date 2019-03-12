#ifndef CONTROL_STACK_PASSIVE_DS
#define CONTROL_STACK_PASSIVE_DS

#include "control_stack.hpp"

typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> Mat;
typedef Eigen::Matrix<double,Eigen::Dynamic,1> Vec;

static double MINSPEED = 1e-6;
static double FLOATEQUAL = 1e-6;

struct ParamsPassiveDS
{
    Eigen::MatrixXd damping_matrix_,
                    basis_matrix_,
                    eig_matrix_;
    int state_dim_,
        num_eigval_;
};

struct InputPassiveDS
{
    Eigen::VectorXd current_velocity_,
                    desired_velocity_;
};

struct OutputPassiveDS
{
    Eigen::VectorXd effort_;
};


namespace control_stack
{
    namespace controllers
    {
        class PassiveDS : public control_stack::ControlStack<InputPassiveDS,OutputPassiveDS,ParamsPassiveDS>
        {
        public:
            PassiveDS() {}
            
            ~PassiveDS() {}

            template <typename... Args> PassiveDS(unsigned int dim, Args... args)
            {
                // Set input state dimension and eigenvalues set
                state_.num_eigval_ = 0;
                state_.state_dim_ = dim;

                // Fill the eigenvalue matrix
                state_.eig_matrix_ = Eigen::MatrixXd::Zero(dim,dim);
                
                AddEigval(args...);
                
                for(size_t i=state_.num_eigval_; i<dim; i++)
                    state_.eig_matrix_(i,i) = state_.eig_matrix_(state_.num_eigval_-1,state_.num_eigval_-1);          
                
                // Initialize the basis matrix
                state_.basis_matrix_ = Eigen::MatrixXd::Random(dim,dim);
                Orthonormalize();
                AssertOrthonormalize();

                // Initialize the damping matrix
                state_.damping_matrix_ = Eigen::MatrixXd::Zero(dim,dim);
            }

            void SetInput(Eigen::VectorXd current, Eigen::VectorXd desired)
            {
                q_.current_velocity_ = current;
                q_.desired_velocity_ = desired;
                Update();
            }

        protected:
            void AddEigval(double T)
            {
                state_.eig_matrix_(state_.num_eigval_,state_.num_eigval_) = T;
                state_.num_eigval_++;
            }
            
            template <typename... Args> void AddEigval(double T, Args... args)
            {
                state_.eig_matrix_(state_.num_eigval_,state_.num_eigval_) = T;
                state_.num_eigval_++;
                AddEigval(args...);
            }

            void Update() override;

            void Orthonormalize();
            
            void AssertOrthonormalize();
            
            void ComputeOrthonormalBasis();
            
            void ComputeDamping();

            // Missing function for set damping eigenvalue
            
        };

    } // namaspace controller

} // namespace controller

#endif // CONTROL_STACK_PASSIVE_DS