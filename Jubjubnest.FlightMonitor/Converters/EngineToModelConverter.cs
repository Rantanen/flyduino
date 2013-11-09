using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace Jubjubnest.FlightControl.Converters
{
	class EngineToModelConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			var engines = value as IEnumerable<EngineViewModel>;
			if (engines == null)
				return null;

			var collection = new Model3DCollection();
			foreach (var engine in engines)
			{
				collection.Add(ConvertOne(engine));
			}

			return collection;
		}

		public GeometryModel3D ConvertOne( EngineViewModel engine )
		{
			var white = Color.FromRgb(255, 255, 255);
			var whiteBrush = new SolidColorBrush(white);
			var model = new GeometryModel3D(GetGeometry(), new DiffuseMaterial(whiteBrush));

			var transform = new Transform3DGroup();
			var scaleTransform = new ScaleTransform3D();
			var translateTransform = new TranslateTransform3D();

			transform.Children.Add(scaleTransform);
			transform.Children.Add(translateTransform);

			scaleTransform.ScaleX = 3;
			scaleTransform.ScaleY = 3;

			var powerBinding = new Binding("Power") {Mode = BindingMode.OneWay, Source = engine};
			BindingOperations.SetBinding(
				scaleTransform,
				ScaleTransform3D.ScaleZProperty,
				powerBinding);

			translateTransform.OffsetX = engine.OffsetX;
			translateTransform.OffsetY = engine.OffsetY;

			model.Transform = transform;

			return model;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		private Geometry3D GetGeometry()
		{

			var mesh = new MeshGeometry3D();
			mesh.Positions = new Point3DCollection
				{
					new Point3D(-0.5, -0.5, 0),
					new Point3D(0.5, -0.5, 0),
					new Point3D(0.5, 0.5, 0),
					new Point3D(-0.5, 0.5, 0),

					new Point3D(-0.5, -0.5, 0.1),
					new Point3D(0.5, -0.5, 0.1),
					new Point3D(0.5, 0.5, 0.1),
					new Point3D(-0.5, 0.5, 0.1)
				};

			mesh.TriangleIndices = new System.Windows.Media.Int32Collection
				{
					0, 3, 1,  1, 3, 2,
					0, 4, 3,  4, 7, 3,
					4, 6, 7,  4, 5, 6,
					0, 1, 4,  1, 5, 4,
					1, 2, 6,  6, 5, 1,
					2, 3, 7,  7, 6, 2
				};

			return mesh;
		}
	}
}
