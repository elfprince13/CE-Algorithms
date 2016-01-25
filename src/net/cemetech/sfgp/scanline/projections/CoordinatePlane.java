package net.cemetech.sfgp.scanline.projections;

import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class CoordinatePlane{

	public static Projection onto(CoordName cP, int vP)
	{
		final CoordName c = cP;
		final int v = vP;
		return new Projection(){
			@Override
			public Point projectPoint(Point p) {
				return new Point(c == CoordName.X ? v : p.getComponent(CoordName.X),
								c == CoordName.Y ? v : p.getComponent(CoordName.Y),
								c == CoordName.Z ? v : p.getComponent(CoordName.Z));
			}
		};
	}

}
