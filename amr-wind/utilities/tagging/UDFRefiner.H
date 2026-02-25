#ifndef UDFREFINER_H
#define UDFREFINER_H

#include <AMReX_Array4.H>
#include <AMReX_Box.H>
#include <AMReX_RealBox.H>
#include <AMReX_TagBox.H>
#include <string>

#include "amr-wind/CFDSim.H"
#include "amr-wind/utilities/tagging/GeometryRefinement.H"
#include "AMReX_Parser.H"

namespace amr_wind {
class CFDSim;
} // namespace amr_wind

namespace amr_wind::tagging {

/** AMR refinement using a user defined expression
 *  \ingroup amr_utils
 *
 */
class UDFRefiner : public GeometryType::Register<UDFRefiner>
{
public:
    static std::string identifier() { return "udf"; }

    explicit UDFRefiner(const CFDSim& sim, const std::string& key);

    ~UDFRefiner() override = default;

    void operator()(
        const amrex::Box& /*bx*/,
        const amrex::Geometry& geom,
        const amrex::Array4<amrex::TagBox::TagType>& tags) const override;

    [[nodiscard]] const amrex::RealBox& bound_box() const override
    {
        return m_bound_box;
    };

private:
    const CFDSim& m_sim;
    amrex::RealBox m_bound_box;
    amrex::Parser m_parser;
};

} // namespace amr_wind::tagging

#endif /* UDFREFINER_H */
