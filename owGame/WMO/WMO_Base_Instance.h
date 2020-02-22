#pragma once

#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

class ZN_API CWMO_Base_Instance 
	: public SceneNode3D
	, public CLoadableObject
{
public:
	typedef std::vector<std::weak_ptr<CWMO_Group_Instance>> GroupInstances;

public:
	CWMO_Base_Instance(const CWMO& WMOObject);
	virtual ~CWMO_Base_Instance();

	void CreateInstances();

	// CLoadableObject
	bool Load() override;

	// CWMO_Base_Instance
	const CWMO& getWMO() const;

	void AddGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_GroupInstances.push_back(_group); }
	const GroupInstances& getGroupInstances() const { return m_GroupInstances; }

	void AddOutdoorGroupInstance(const std::weak_ptr<CWMO_Group_Instance>& _group) { m_OutdoorGroupInstances.push_back(_group); }
	const GroupInstances& getGroupOutdoorInstances() const { return m_OutdoorGroupInstances; }

	// SceneNode3D
	virtual void Initialize() override;
	std::string GetName() const override;
	void Update(const UpdateEventArgs& e) override;
	void Accept(IVisitor* visitor) override;

protected:
	const CWMO&                           m_WMOObject;
	//SWMO_Doodad_SetInfo                           m_DoodadSetInfo;
	
	GroupInstances  m_GroupInstances;
	GroupInstances  m_OutdoorGroupInstances;
};