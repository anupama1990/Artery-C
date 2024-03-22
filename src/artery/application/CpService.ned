//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package artery.application;

simple CpService like ItsG5Service
{
    parameters:
        @signal[CpmReceived](type=CpObject);
        @signal[CpmSent](type=CpObject);
        @signal[CPMSentTime](type=simtime_t);
        @signal[EteDelay](type=simtime_t);
        @signal[objectAge](type=simtime_t);
        @signal[timebwupdate](type=simtime_t);
        @signal[ear](type=double);
        @signal[numobjAR](type=long);
        @signal[numobjCPMrcvd](type=long);
        @signal[numobjCPMsent](type=long);
        @signal[msgsize](type=long);
        @signal[periodicity](type=simtime_t);
        @signal[numCPMPerSec](type=long);


        @statistic[receptioncpm](source=CpmReceived;record=vector(cpmStationId)?,vector(cpmGenerationDeltaTime)?);
        @statistic[transmissioncpm](source=CpmSent;record=vector(cpmStationId)?,vector(cpmGenerationDeltaTime)?);
        @statistic[sentcpmpkt](source=count(CpmSent);record=last,vector);
        @statistic[rcvdcpmpkt](source=count(CpmReceived);record=last,vector);
        @statistic[CPMSentTime](source=CPMSentTime; record = vector);
        @statistic[EteDelay](source=EteDelay; record = stats,vector);
        @statistic[EAR](source=ear; record = stats?,vector);
        @statistic[numobjAR](source=numobjAR; record=vector);
        @statistic[numobjCPMrcvd](source=numobjCPMrcvd; record=stats,vector);
        @statistic[numobjCPMsent](source=numobjCPMsent; record=stats,vector);
        @statistic[objectAge](source=objectAge; record=stats,vector);
        @statistic[timebwupdate](source=timebwupdate; record=stats,vector);
        @statistic[msgsize](source=msgsize; record=stats,vector);
        @statistic[periodicity](source=periodicity; record=stats,vector);
        @statistic[numCPMPerSec](source=numCPMPerSec; record=stats,vector,histogram);

        // parameter to enable mode 4
        bool enable_constsize = default(false);

        //parameter for the relevance area
        double maxRadiusRelArea @unit(m) = default(500m);

        // evaluate DCC transmission interval restrictions
        bool withDccRestriction = default(true);

        // generation interval boundaries
        double minInterval @unit(s) = default(0.1s);
        double maxInterval @unit(s) = default(1.0s);

        // generate at fixed rate (using minInterval, optionally restricted by DCC)
        bool fixedRate = default(false);
        double fixedInterval @unit(s) = default(0.1s);

        // change in orientation triggering CAM generation (in degree)
        double headingDelta = default(4.0);

        // change in position triggering CAM generation
        double positionDelta @unit(m) = default(4.0m);

        // change in speed triggering CAM generation (in meter/second)
        double speedDelta @unit(mps) = default(0.5mps);
}
