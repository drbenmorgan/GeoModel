/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DistanceToIn stuff for LArWheelSliceSolid
#include <cassert>
//#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "GeoModel2G4/LArWheelSliceSolid.h"

G4double LArWheelSliceSolid::DistanceToIn(const G4ThreeVector &inputP) const
{
    LWSDBG(1, std::cout << TypeStr() << " DisToIn" << MSG_VECTOR(inputP) << std::endl);
    if(m_BoundingShape->Inside(inputP) == kOutside) {
    // here is an approximation - for the point outside m_BoundingShape
    // the solid looks like a m_BoundingShape
    // it's okay since the result could be underestimated
        LWSDBG(2, std::cout << "Outside BS" << std::endl);
        return m_BoundingShape->DistanceToIn(inputP);
    }
    G4ThreeVector p(inputP);

    //
    // DistanceToTheNearestFan:
    // rotates point p to the localFan coordinates and returns fan number to out_fan_number parameter
    // returns distance to fan as result
    //

    int p_fan = 0;
    const G4double d = fabs(GetCalculator()->DistanceToTheNearestFan(p, p_fan));
    if(d > m_FHTplusT){
        const G4double result = d - m_FanHalfThickness;
        LWSDBG(2, std::cout << "dti result = " << result << std::endl);
        return result;
    }
    LWSDBG(2, std::cout << "already inside, return 0" << MSG_VECTOR(p) << std::endl);
    return 0.;
}

G4double LArWheelSliceSolid::DistanceToIn(const G4ThreeVector &inputP,
                                     const G4ThreeVector &inputV) const
{
    LWSDBG(1, std::cout << TypeStr() << " DisToIn" << MSG_VECTOR(inputP)
                        << MSG_VECTOR(inputV) << std::endl);

    G4double distance = 0.;
    const EInside inside_BS = m_BoundingShape->Inside(inputP);
    G4ThreeVector p(inputP);
    if(inside_BS == kOutside) {
        distance = m_BoundingShape->DistanceToIn(inputP, inputV);
        if(distance == kInfinity) {
            LWSDBG(2, std::cout << "Infinity distance to m_BoundingShape"
                                << MSG_VECTOR(inputP) << MSG_VECTOR(inputV)
                                << std::endl);
            return kInfinity;
        }
        p += inputV * distance;
        assert(m_BoundingShape->Inside(p) != kOutside);
        LWSDBG(2, std::cout << "shift" << MSG_VECTOR(inputP) << std::endl);
    }

    const G4double phi0 = p.phi();
    int p_fan = 0;
    const G4double d = GetCalculator()->DistanceToTheNearestFan(p, p_fan);
    if(fabs(d) < m_FHTminusT){
        LWSDBG(2, std::cout << "already inside fan" << MSG_VECTOR(p) << std::endl);
        // if initial point is on BS surface and inside fan volume it is a real surface
        if(inside_BS == kSurface) {
            LWSDBG(2, std::cout << "On BS surface" << std::endl);
            return m_BoundingShape->DistanceToIn(inputP, inputV);
        }
        return distance;
    }
    G4ThreeVector v(inputV);
    v.rotateZ(p.phi() - phi0);

    const G4double d0 = distance_to_in(p, v, p_fan);
    distance += d0;

#ifdef DEBUG_LARWHEELSLICESOLID
    if(Verbose > 2){
        if(Verbose > 3){
            std::cout << MSG_VECTOR(inputP)
                      << " " << MSG_VECTOR(inputV) << std::endl;
        }
        std::cout << "dti: " << d0 << ", DTI: " << distance << std::endl;
    }
    if(Verbose > 3){
        if(d0 < kInfinity){
            G4ThreeVector p2 = inputP + inputV*distance;
            EInside i = Inside(p2);
            std::cout << "DTI hit at dist. " << distance << ", point "
                      << MSG_VECTOR(p2) << ", "
                      << inside(i) << std::endl;
        } else {
            std::cout << "got infinity from dti" << std::endl;
        }
    }
#ifdef LWS_HARD_TEST_DTI
    if(test_dti(inputP, inputV, distance)){
        if(Verbose == 1){
            std::cout << TypeStr() << " DisToIn" << MSG_VECTOR(inputP)
                      << MSG_VECTOR(inputV) << std::endl;
        }
    }
    if(Verbose == 1){
        std::cout << TypeStr() << " DisToIn" << MSG_VECTOR(inputP)
                  << MSG_VECTOR(inputV) << " " << distance << std::endl;
    }
#endif // ifdef LWS_HARD_TEST_DTI

#endif // ifdef DEBUG_LARWHEELSLICESOLID

  return distance;
}

G4double LArWheelSliceSolid::distance_to_in(G4ThreeVector &p, const G4ThreeVector &v, int p_fan) const
{
    LWSDBG(4, std::cout << "dti:           " << MSG_VECTOR(p) << " "
                        << MSG_VECTOR(v) << std::endl);

    G4double distance = 0.;

    if(p.x() > m_Xmax) {
        if(v.x() >= 0.) return kInfinity;
        const G4double b = (m_Xmax - p.x()) / v.x();
        const G4double y2 = p.y() + v.y() * b;
        const G4double z2 = p.z() + v.z() * b;
        p.set(m_Xmax, y2, z2);
        distance += b;
    } else if(p.x() < m_Xmin) {
        if(v.x() <= 0.)    return kInfinity;
        const G4double b = (m_Xmin - p.x()) / v.x();
        const G4double y2 = p.y() + v.y() * b;
        const G4double z2 = p.z() + v.z() * b;
        p.set(m_Xmin, y2, z2);
        distance += b;
    }

// here p is on surface of or inside the "FanBound",
// distance corrected, misses are accounted for
    LWSDBG(5, std::cout << "dti corrected: " << MSG_VECTOR(p) << std::endl);

    G4double dist_p = GetCalculator()->DistanceToTheNeutralFibre(p, p_fan);
    if(fabs(dist_p) < m_FHTminusT) {
        LWSDBG(5, std::cout << "hit fan dist_p=" << dist_p << ", m_FHTminusT=" << m_FHTminusT << std::endl);
        return distance;
    }

    G4ThreeVector q;
    q = p + v * m_BoundingShape->DistanceToOut(p, v);
    G4double dist_q = GetCalculator()->DistanceToTheNeutralFibre(q, p_fan);
    LWSDBG(5, std::cout << "dti exit point: " << MSG_VECTOR(q) << " "
                        << dist_q << std::endl);
    G4double dd = kInfinity;
    if(dist_p * dist_q < 0.){// it certanly cross current half-wave
        dd = in_iteration_process(p, dist_p, q, p_fan);
    }
    G4double d2 = search_for_nearest_point(p, dist_p, q, p_fan);
    if(d2 < kInfinity){
        return distance + d2; // this half-wave is intersected
    } else if(dd < kInfinity){
        return distance + dd;
    }
    return kInfinity;
}

// This functions should be called in the case when we are sure that
// points p (which sould be OUTSIDE of vertical fan) and out_point have
// the surface of the vertical fan between them.
// returns distance from point p to absorber surface
// sets last parameter to the founded point
G4double LArWheelSliceSolid::in_iteration_process(
    const G4ThreeVector &p, G4double dist_p, G4ThreeVector &B, int p_fan
) const
{
    LWSDBG(6, std::cout << "iip from " << p << " to " << B
                        << " dir " << (B - p).unit()
                        << std::endl);

    G4ThreeVector A, C, diff;
    A = p;
    G4double dist_c;
    unsigned int niter = 0;
  //  assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(A)) > m_FHTplusT);
  //  assert(GetCalculator()->DistanceToTheNeutralFibre(A) == dist_p);
    do {
        C = A + B;
        C *= 0.5;
        dist_c = GetCalculator()->DistanceToTheNeutralFibre(C, p_fan);
        if(dist_c * dist_p < 0. || fabs(dist_c) < m_FHTminusT){
            B = C;
        } else {
            A = C;
        }
        niter ++;
        diff = A - B;
    } while(diff.mag2() > s_IterationPrecision2 && niter < s_IterationsLimit);
    assert(niter < s_IterationsLimit);
    assert(fabs(GetCalculator()->DistanceToTheNeutralFibre(B, p_fan)) < m_FHTplusT);
    diff = p - B;
    LWSDBG(7, std::cout << "iip result in " << niter << " = " << B
                        << " " << diff.mag() << std::endl);
    return diff.mag();
}

// search for the nearest to the neutral fibre of the vertical fan point
// on the segment between p_in and p_out
G4double LArWheelSliceSolid::search_for_nearest_point(
    const G4ThreeVector &p_in, const G4double dist_p_in,
    const G4ThreeVector &p_out, int p_fan
) const
{
    LWSDBG(6, std::cout << "sfnp " << MSG_VECTOR(p_in) << " "
                        << MSG_VECTOR(p_out) << std::endl);

    G4ThreeVector A, B, C, l, diff;
    A = p_in;
    B = p_out;
    diff = B - A;
    l = diff.unit() * s_IterationPrecision;
  // this is to correctly take the sign of the distance into account
    G4double sign = dist_p_in < 0.? -1. : 1.;
    G4double d_prime;
    G4double dist_c;
    unsigned long niter = 0;
    do {
        C = A + B;
        C *= 0.5;
        dist_c = GetCalculator()->DistanceToTheNeutralFibre(C, p_fan);
        if(dist_c * sign <= 0.){ // we are in coditions for in_iteration_process
            LWSDBG(7, std::cout << "sfnp0 " << dist_c << std::endl);
            return in_iteration_process(p_in, dist_p_in, C, p_fan);
        }
    // calculate sign of derivative of distance to the neutral fibre
    // hope this substitution is acceptable
        diff = C - l;
        d_prime = (dist_c - GetCalculator()->DistanceToTheNeutralFibre(diff, p_fan)) * sign;
        if(d_prime < 0.) A = C;
        else B = C;
        niter ++;
        diff = A - B;
    } while(diff.mag2() > s_IterationPrecision2 && niter < s_IterationsLimit);
    assert(niter < s_IterationsLimit);
    if(fabs(dist_c) < m_FHTminusT){
        LWSDBG(7, std::cout << "sfnp1 " << dist_c << std::endl);
        return in_iteration_process(p_in, dist_p_in, C, p_fan);
    }
  //  let's check at p_in and p_out
    if(dist_p_in * sign < dist_c * sign){
        C = p_in;
        dist_c = dist_p_in;
    }
    G4double dist_p_out = GetCalculator()->DistanceToTheNeutralFibre(p_out, p_fan);
    if(dist_p_out *sign < dist_c * sign) C = p_out;
    if(fabs(dist_p_out) < m_FHTminusT){
        LWSDBG(7, std::cout << "sfnp2 " << dist_p_out << std::endl);
        return in_iteration_process(p_in, dist_p_in, C, p_fan);
    }
    return kInfinity;
}

