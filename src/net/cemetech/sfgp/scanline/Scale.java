package net.cemetech.sfgp.scanline;

public class Scale extends Projection {
	int scale;
	public Scale(int s){scale = s;}

	@Override
	public Point projectPoint(Point p) {
		return new Point(
				scale*p.getComponent(Point.CoordName.X),
				scale*p.getComponent(Point.CoordName.Y),
				scale*p.getComponent(Point.CoordName.Z)
				);
	}

}
