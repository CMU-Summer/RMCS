#ifndef GROUP_COMMAND_HPP
#define GROUP_COMMAND_HPP

#include "hebi_group_command.h"
#include "Eigen/Eigen"
#include "command.hpp"
#include <vector>

namespace hebi {

/**
 * \brief A list of Command objects appropriate for sending to a Group of
 * modules; the size() must match the number of modules in the group.
 */
class GroupCommand final
{
  public:
    #ifndef DOXYGEN_OMIT_INTERNAL
    /**
     * C-style group command object.
     * NOTE: this should not be used except by library functions!
     */
    HebiGroupCommandPtr const internal_;
    #endif // DOXYGEN_OMIT_INTERNAL

  private:
    /**
     * True if this object is responsible for creating and destroying the
     * underlying C pointer; false otherwise.
     */
    const bool manage_pointer_lifetime_;
    /**
     * The number of modules in this group command.
     */
    const int number_of_modules_;
    /**
     * The list of Command subobjects
     */
    std::vector<Command> commands_;

  public:
    /**
     * \brief Create a group command with the specified number of modules.
     */
    GroupCommand(int number_of_modules);

    /**
     * \brief Destructor cleans up group command object as necessary.
     */
    virtual ~GroupCommand() noexcept; /* annotating specified destructor as noexcept is best-practice */

    /**
     * \brief Returns the number of module commands in this group command.
     */
    int size() const;

    /**
     * \brief Access the command for an individual module.
     */
    Command& operator[](int index);

    /**
     * \brief Convenience function for setting position commands from Eigen
     * vectors.
     *
     * Note that if the vector is not the correct link, no action is taken.
     */
    void setPosition(const Eigen::VectorXd& position);
    /**
     * \brief Convenience function for setting velocity commands from Eigen
     * vectors.
     *
     * Note that if the vector is not the correct link, no action is taken.
     */
    void setVelocity(const Eigen::VectorXd& velocity);
    /**
     * \brief Convenience function for setting torque commands from Eigen
     * vectors.
     *
     * Note that if the vector is not the correct link, no action is taken.
     */
    void setTorque(const Eigen::VectorXd& torque);

    /**
     * \brief Convenience function for returning commanded position values.
     */
    Eigen::VectorXd getPosition() const;
    /**
     * \brief Convenience function for returning commanded velocity values.
     */
    Eigen::VectorXd getVelocity() const;
    /**
     * \brief Convenience function for returning commanded torque values.
     */
    Eigen::VectorXd getTorque() const;
};

} // namespace hebi

#endif // GROUP_COMMAND_HPP
