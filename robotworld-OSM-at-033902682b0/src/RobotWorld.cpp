#include "RobotWorld.hpp"

#include "Goal.hpp"
#include "Logger.hpp"
#include "Robot.hpp"
#include "Wall.hpp"
#include "WayPoint.hpp"
#include "MessageTypes.hpp"

#include <algorithm>

namespace Model
{
	RobotWorld::RobotWorld() :localRobot(nullptr)
	{

	}

	/**
	 *
	 */
	/* static */RobotWorld& RobotWorld::RobotWorld::getRobotWorld()
	{
		static RobotWorld robotWorld;
		return robotWorld;
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::newRobot(const std::string& aName /*= "New Robot"*/,
	        const wxPoint& aPosition /*= wxPoint(-1,-1)*/,
	        bool aNotifyObservers /*= true*/)
	{
		RobotPtr robot = std::make_shared<Robot>(aName, aPosition);
		robots.push_back(robot);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return robot;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::newWayPoint(
	        const std::string& aName /*= "new WayPoint"*/,
	        const wxPoint& aPosition /*= wxPoint(-1,-1)*/,
	        bool aNotifyObservers /*= true*/)
	{
		WayPointPtr wayPoint(new WayPoint(aName, aPosition));
		wayPoints.push_back(wayPoint);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return wayPoint;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::newGoal(const std::string& aName /*= "New Goal"*/,
	        const wxPoint& aPosition /*= wxPoint(-1,-1)*/,
	        bool aNotifyObservers /*= true*/)
	{
		GoalPtr goal = std::make_shared<Goal>(aName, aPosition);
		goals.push_back(goal);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return goal;
	}
	/**
	 *
	 */
	WallPtr RobotWorld::newWall(const wxPoint& aPoint1, const wxPoint& aPoint2,
	        bool aNotifyObservers /*= true*/)
	{
		WallPtr wall = std::make_shared<Wall>(aPoint1, aPoint2);
		walls.push_back(wall);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return wall;
	}
	/**
	 *
	 */
	void RobotWorld::deleteRobot(RobotPtr aRobot,
	        bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if(robots.begin(), robots.end(), [aRobot](RobotPtr r)
		{
			return aRobot->getName() == r->getName();
		});
		if (i != robots.end())
		{
			robots.erase(i);
			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteWayPoint(WayPointPtr aWayPoint,
	        bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if(wayPoints.begin(), wayPoints.end(),
		        [aWayPoint](WayPointPtr w)
		        {
			        return aWayPoint->getName() == w->getName();
		        });
		if (i != wayPoints.end())
		{
			wayPoints.erase(i);
			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteGoal(GoalPtr aGoal, bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if(goals.begin(), goals.end(), [aGoal](GoalPtr g)
		{
			return aGoal->getName() == g->getName();
		});
		if (i != goals.end())
		{
			goals.erase(i);

			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteWall(WallPtr aWall, bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if(walls.begin(), walls.end(), [aWall](WallPtr w)
		{
			return
			aWall->getPoint1() == w->getPoint1() &&
			aWall->getPoint2() == w->getPoint2();
		});
		if (i != walls.end())
		{
			walls.erase(i);

			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::getRobot(const std::string& aName) const
	{
		if (auto i = std::find_if(robots.begin(), robots.end(),
		        [&aName](RobotPtr robot)
		        {	return robot->getName() == aName;}); i != robots.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::getRobot(const Base::ObjectId& anObjectId) const
	{
		if (auto i = std::find_if(robots.begin(), robots.end(),
		        [&anObjectId](RobotPtr robot)
		        {	return robot->getObjectId() == anObjectId;}); i
		        != robots.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::getWayPoint(const std::string& aName) const
	{
		if (auto i = std::find_if(wayPoints.begin(), wayPoints.end(),
		        [&aName](WayPointPtr wayPoint)
		        {	return wayPoint->getName() == aName;}); i
		        != wayPoints.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::getWayPoint(const Base::ObjectId& anObjectId) const
	{
		if (auto i = std::find_if(wayPoints.begin(), wayPoints.end(),
		        [&anObjectId](WayPointPtr wayPoint)
		        {	return wayPoint->getObjectId() == anObjectId;}); i
		        != wayPoints.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::getGoal(const std::string& aName) const
	{
		if (auto i = std::find_if(goals.begin(), goals.end(),
		        [&aName](GoalPtr goal)
		        {	return goal->getName() == aName;}); i != goals.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::getGoal(const Base::ObjectId& anObjectId) const
	{
		if (auto i = std::find_if(goals.begin(), goals.end(),
		        [&anObjectId](GoalPtr goal)
		        {	return goal->getObjectId() == anObjectId;}); i
		        != goals.end())
		{
			return *i;
		}
		return nullptr;
	}
	/**
	 *
	 */
	WallPtr RobotWorld::getWall(const Base::ObjectId& anObjectId) const
	{
		if (auto i = std::find_if(walls.begin(), walls.end(),
		        [&anObjectId](WallPtr wall)
		        {	return wall->getObjectId() == anObjectId;}); i
		        != walls.end())
		{
			return *i;
		}
		return nullptr;
	}

	/**
	 *
	 */
	const std::vector<RobotPtr>& RobotWorld::getRobots() const
	{
		return robots;
	}
	/**
	 *
	 */
	const std::vector<WayPointPtr>& RobotWorld::getWayPoints() const
	{
		return wayPoints;
	}
	/**
	 *
	 */
	const std::vector<GoalPtr>& RobotWorld::getGoals() const
	{
		return goals;
	}
	/**
	 *
	 */
	const std::vector<WallPtr>& RobotWorld::getWalls() const
	{
		return walls;
	}
	/**
	 *
	 */
	void populateWorld0(bool fromRequest)
	{
		RobotWorld& robotWorld = RobotWorld::getRobotWorld();
				if (fromRequest)
				{
					robotWorld.newRobot("Peer", wxPoint(50, 60), false);
					robotWorld.newGoal("PeerGoal", wxPoint(450, 440), false);
				} else
				{
					Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
					robot->setPosition(wxPoint(50, 60), false);
					robotWorld.newGoal("YourGoal", wxPoint(450, 440), false);
				}
	}
	/**
	 *
	 */
	void populateWorld1(bool fromRequest)
	{
		RobotWorld& robotWorld = RobotWorld::getRobotWorld();
		if (fromRequest)
		{
			robotWorld.newRobot("Peer", wxPoint(450, 450), false);
			robotWorld.newGoal("PeerGoal", wxPoint(50, 60), false);
		} else
		{
			Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
			robot->setPosition(wxPoint(450, 450), false);
			robotWorld.newGoal("YourGoal", wxPoint(50, 60), false);
		}
	}
	/**
	 *
	 */
	void populateWorld2(bool fromRequest)
	{
		RobotWorld& robotWorld = RobotWorld::getRobotWorld();
		if (fromRequest)
		{
			robotWorld.newRobot("Peer", wxPoint(40, 40), false);
			robotWorld.newGoal("PeerGoal", wxPoint(480, 480), false);
		} else
		{
			Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
			robot->setPosition(wxPoint(20, 20), false);
			robotWorld.newGoal("YourGoal", wxPoint(480, 480), false);
		}
	}
	/**
	 *
	 */
	void populateWorld3(bool fromRequest)
		{
			RobotWorld& robotWorld = RobotWorld::getRobotWorld();
			if (fromRequest)
			{
				robotWorld.newRobot("Peer", wxPoint(480, 40), false);
				robotWorld.newGoal("PeerGoal", wxPoint(40, 480), false);
			} else
			{
				Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
				robot->setPosition(wxPoint(480, 40), false);
				robotWorld.newGoal("YourGoal", wxPoint(40, 480), false);
			}
		}
	/**
	 *
	 */
	void populateWorld4(bool fromRequest)
		{
			populateWorld0(fromRequest);
			RobotWorld::getRobotWorld().newWall(wxPoint(480, 300), wxPoint(150, 300), false);
		}
	/**
	 *
	 */
	void populateWorld5(bool fromRequest)
		{
			populateWorld1(fromRequest);
			RobotWorld::getRobotWorld().newWall(wxPoint(20, 100), wxPoint(350, 100), false);
		}
	/**
	 *
	 */
	void RobotWorld::populate(int worldNumber, bool fromRequest)
	{
		switch(worldNumber)
		{
			case 0:
			{
				populateWorld0(fromRequest);
				break;
			}

			case 1:
			{
				populateWorld1(fromRequest);
				break;
			}

			case 2:
			{
				populateWorld2(fromRequest);
				break;
			}

			case 3:
			{
				populateWorld3(fromRequest);
				break;
			}

			case 4:
			{
				populateWorld4(fromRequest);
				break;
			}
			case 5:
			{
				populateWorld5(fromRequest);
				break;
			}
			default:
			{
				break;
			}
		}

		notifyObservers();
	}
	/**
	 *
	 */
	void RobotWorld::unpopulate(bool aNotifyObservers /*= true*/)
	{
		localRobot = nullptr;
		robots.clear();
		wayPoints.clear();
		goals.clear();
		walls.clear();

		if (aNotifyObservers)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	void RobotWorld::unpopulate(const std::vector<Base::ObjectId>& aKeepObjects,
	        bool aNotifyObservers /*= true*/)
	{
		if (robots.size() > 0)
		{
			robots.erase(
			        std::remove_if(robots.begin(), robots.end(),
			                [&aKeepObjects](
			                        RobotPtr aRobot)
			                        {
				                        return std::find( aKeepObjects.begin(),
						                        aKeepObjects.end(),
						                        aRobot->getObjectId()) == aKeepObjects.end();
			                        }), robots.end());
		}
		if (wayPoints.size() > 0)
		{
			wayPoints.erase(
			        std::remove_if(wayPoints.begin(), wayPoints.end(),
			                [&aKeepObjects](
			                        WayPointPtr aWayPoint)
			                        {
				                        return std::find( aKeepObjects.begin(),
						                        aKeepObjects.end(),
						                        aWayPoint->getObjectId()) == aKeepObjects.end();
			                        }), wayPoints.end());
		}
		if (goals.size() > 0)
		{
			goals.erase(
			        std::remove_if(goals.begin(), goals.end(),
			                [&aKeepObjects](
			                        GoalPtr aGoal)
			                        {
				                        return std::find( aKeepObjects.begin(),
						                        aKeepObjects.end(),
						                        aGoal->getObjectId()) == aKeepObjects.end();
			                        }), goals.end());
		}
		if (walls.size() > 0)
		{
			walls.erase(
			        std::remove_if(walls.begin(), walls.end(),
			                [&aKeepObjects](
			                        WallPtr aWall)
			                        {
				                        return std::find( aKeepObjects.begin(),
						                        aKeepObjects.end(),
						                        aWall->getObjectId()) == aKeepObjects.end();
			                        }), walls.end());
		}

		if (aNotifyObservers)
		{
			notifyObservers();
		}
	}
	std::string RobotWorld::asCode() const
	{
		std::ostringstream os;
		os << "\n\n";
		for (RobotPtr ptr : robots)
		{
			os << "RobotWorld::getRobotWorld().newRobot( \"" << ptr->getName()
			        << "\", wxPoint(" << ptr->getPosition().x << ","
			        << ptr->getPosition().y << "),false);\n";
		}
		for (WallPtr ptr : walls)
		{
			os << "RobotWorld::getRobotWorld().newWall( " << "wxPoint("
			        << ptr->getPoint1().x << "," << ptr->getPoint1().y << "),"
			        << "wxPoint(" << ptr->getPoint2().x << ","
			        << ptr->getPoint2().y << "),false);\n";
		}
		for (WayPointPtr ptr : wayPoints)
		{
			os << "RobotWorld::getRobotWorld().newWayPoint( \""
			        << ptr->getName() << "\", wxPoint(" << ptr->getPosition().x
			        << "," << ptr->getPosition().y << "),false);\n";
		}
		for (GoalPtr ptr : goals)
		{
			os << "RobotWorld::getRobotWorld().newGoal( \"" << ptr->getName()
			        << "\", wxPoint(" << ptr->getPosition().x << ","
			        << ptr->getPosition().y << "),false);\n";
		}
		os << "\n\n";
		return os.str();
	}
	/**
	 *
	 */
	std::string RobotWorld::asString() const
	{
		return ModelObject::asString();
	}
	/**
	 *
	 */
	std::string RobotWorld::asDebugString() const
	{
		std::ostringstream os;

		os << asString() << '\n';

		for (RobotPtr ptr : robots)
		{
			os << ptr->asDebugString() << '\n';
		}
		for (WayPointPtr ptr : wayPoints)
		{
			os << ptr->asDebugString() << '\n';
		}
		for (GoalPtr ptr : goals)
		{
			os << ptr->asDebugString() << '\n';
		}
		for (WallPtr ptr : walls)
		{
			os << ptr->asDebugString() << '\n';
		}

		return os.str();
	}
	
	RobotPtr RobotWorld::getLocalRobot()
	{
		if (localRobot == nullptr) {
			localRobot = newRobot("You", wxPoint(-100,-100), false);
		}
		return localRobot;
	}

	/**
	 *
	 */
	RobotWorld::~RobotWorld()
	{
		// No notification while I am in the destruction mode!
		disableNotification();
		unpopulate();
	}

}    // namespace Model
