/*
 * Copyright (c) 2020 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Ananthakrishnan S <ananthakrishnan190@gmail.com>
 *         Tom Henderson <tomh@tomh.org>
 */

#ifndef NET_DEVICE_STATE_H
#define NET_DEVICE_STATE_H

#include "ns3/callback.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

namespace ns3
{

/**
 * \ingroup network
 * \brief Administrative and Operational state of NetDevice.
 *
 * This class holds the implementation of administrative state and
 * operational state of a NetDevice. Operational state is based on
 * the states mentioned in RFC 2863: The Interfaces Group MIB.
 * Administrative state is represented by a boolean variable (up or down).
 * This class can be subclassed to provide implementations specific to
 * NetDevices. However, anyone wanting to use this architecture should
 * use public APIs in the base class itself. This implementation is not
 * a necessary part of NetDevice. In other words, this is an optional feature.
 *
 * Upper layers such as IP that are interested in keeping track of states
 * of NetDevice can connect to traced callbacks in this class.
 */
class NetDeviceState : public Object
{
  public:
    /**
     * \brief This enum is the implementation of RFC 2863 operational states.
     *
     * More details can be found here:
     * https://tools.ietf.org/html/rfc2863
     *
     * The numbers assigned to the members in the enum is according to the
     * documentation in the below link:
     * https://www.kernel.org/doc/Documentation/networking/operstates.txt
     */
    enum OperationalState
    {
        /**
         * Carrier is down on a non-stacked device or device
         * is admin down.
         */
        IF_OPER_DOWN = 2,

        /**
         * Useful only in stacked interfaces. An interface stacked on
         * another interface that is in IF_OPER_DOWN show this state.
         * (eg. VLAN)
         */
        IF_OPER_LOWERLAYERDOWN = 3,

        /**
         * Interface is L1 up, but waiting for an external event, for eg. for a
         * protocol to establish such as 802.1X.
         */
        IF_OPER_DORMANT = 5,

        /**
         * Carrier is detected and the device can be used.
         */
        IF_OPER_UP = 6,
    };

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Constructor
     */
    NetDeviceState();

    /**
     * \return RFC 2863 operational state of the NetDevice.
     */
    OperationalState GetOperationalState() const;

    /**
     * \brief Set RFC 2863 operational state of the device.
     * This method even though public is intended to be
     * used only by associated NetDevice and Channel used
     * by that NetDevice.
     *
     * \param opState the state to be set.
     */
    void SetOperationalState(OperationalState opState);

    /**
     * \brief Check the administrative state of the NetDevice
     * \return true if the administrative state of the NetDevice is up.
     */
    bool IsUp() const;

    /**
     * \brief Set the NetDevice to an (administratively) up state.
     */
    void SetUp();

    /**
     * \brief Set the NetDevice to an (administratively) down state.
     * This method also sets the operational state of the
     * device to IF_OPER_DOWN.
     */
    void SetDown();

    /**
     * \brief TracedCallback signature for state changes
     * \param [in] isUp Whether the administrative state is up
     * \param [in] opState The operational state of the device
     */
    typedef void (*StateChangedTracedCallback)(bool isUp, OperationalState opState);

  private:
    /**
     * \brief This method is used to take care of device specific
     *  actions needed to bring up a NetDevice similar to calling
     *  ndo_open () from dev_open () in Linux.
     */
    virtual void DoSetUp();

    /**
     * \brief This method is used to take care of device specific
     *  actions needed to bring down a NetDevice similar to calling
     *  ndo_stop () from inside dev_close () in Linux.
     */
    virtual void DoSetDown();

    /**
     * \brief This method is used to take care of device specific
     *  actions needed to change operational state (if needed).
     * \param opState OperationalState to set
     */
    virtual void DoSetOperationalState(OperationalState opState);

  protected:
    /**
     * \brief Implementation of virtual function.
     */
    void DoInitialize() override;

    /**
     * \brief Traced callback for tracing device states.
     */
    TracedCallback<bool, OperationalState> m_stateChangeTrace;

  private:
    /**
     * \brief Represents IFF_UP in net_device_flags enum
     * in Linux. Used to store the administrative
     * state of the NetDevice.
     */
    bool m_isUp;

    /**
     * \brief RFC 2863 operational state of the device.
     */
    OperationalState m_operationalState;
};

/**
 * \brief Stream insertion operator.
 *
 * \param os the stream
 * \param state the RFC 2863 operational state
 * \returns a reference to the stream
 */
inline std::ostream&
operator<<(std::ostream& os, NetDeviceState::OperationalState state)
{
    switch (state)
    {
    case NetDeviceState::IF_OPER_DOWN:
        return (os << "IF_OPER_DOWN");

    case NetDeviceState::IF_OPER_UP:
        return (os << "IF_OPER_UP");

    case NetDeviceState::IF_OPER_DORMANT:
        return (os << "IF_OPER_DORMANT");

    case NetDeviceState::IF_OPER_LOWERLAYERDOWN:
        return (os << "IF_OPER_LOWERLAYERDOWN");

    default:
        return (os << "INVALID STATE");
    }
}

} // namespace ns3

#endif /* NET_DEVICE_STATE_H */
