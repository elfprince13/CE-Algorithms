package net.cemetech.sfgp.scanline;

import net.cemetech.sfgp.scanline.Point.CoordName;

public class CoordinatePlane{

	static Projection onto(CoordName cP, int vP)
	{
		final CoordName c = cP;
		final int v = vP;
		return new Projection(){
			@Override
			public Point projectPoint(Point p) {
				// TODO Auto-generated method stub
				return new Point(c == CoordName.X ? v : p.getComponent(CoordName.X),
								c == CoordName.Y ? v : p.getComponent(CoordName.Y),
								c == CoordName.Z ? v : p.getComponent(CoordName.Z));
			}
		};
	}

}
