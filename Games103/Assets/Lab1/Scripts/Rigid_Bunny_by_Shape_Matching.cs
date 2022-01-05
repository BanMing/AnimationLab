using System.Threading.Tasks;
using UnityEngine;

public class Rigid_Bunny_by_Shape_Matching : MonoBehaviour
{
    private const float linear_decay = 0.999f; // for velocity decay
    private const float min_velocity = 0.1f;
    private const float un = 0.3f; // spring factor  [0,1]
    private const float ut = 0.5f; // friction factor
    private const float dt = 0.015f;

    private readonly Vector3 g = new Vector3(0, -9.8f, 0);

    public bool launched = false;

    private float inv_dt = 0;
    private Vector3[] X;
    private Vector3[] Q;
    private Vector3[] V;
    private Vector3 c;
    private Matrix4x4 A;
    private Matrix4x4 QQt = Matrix4x4.zero;
    private Matrix4x4 QQt_i = Matrix4x4.zero;

    // Start is called before the first frame update
    private void Start()
    {
        Mesh mesh = GetComponent<MeshFilter>().mesh;
        V = new Vector3[mesh.vertices.Length];
        X = mesh.vertices;
        Q = mesh.vertices;

        //Centerizing Q.
        Vector3 c = Vector3.zero;
        for (int i = 0; i < Q.Length; i++)
            c += Q[i];
        c /= Q.Length;
        for (int i = 0; i < Q.Length; i++)
            Q[i] -= c;

        //Get QQ^t ready.
        for (int i = 0; i < Q.Length; i++)
        {
            QQt[0, 0] += Q[i][0] * Q[i][0];
            QQt[0, 1] += Q[i][0] * Q[i][1];
            QQt[0, 2] += Q[i][0] * Q[i][2];
            QQt[1, 0] += Q[i][1] * Q[i][0];
            QQt[1, 1] += Q[i][1] * Q[i][1];
            QQt[1, 2] += Q[i][1] * Q[i][2];
            QQt[2, 0] += Q[i][2] * Q[i][0];
            QQt[2, 1] += Q[i][2] * Q[i][1];
            QQt[2, 2] += Q[i][2] * Q[i][2];
        }
        QQt[3, 3] = 1;
        QQt_i = QQt.inverse;

        for (int i = 0; i < X.Length; i++)
            V[i][0] = 4.0f;

        Update_Mesh(transform.position, Matrix4x4.Rotate(transform.rotation));

        inv_dt = 1 / dt;
        transform.position = Vector3.zero;
        transform.rotation = Quaternion.identity;
    }

    // Polar Decomposition that returns the rotation from F.
    private Matrix4x4 Get_Rotation(Matrix4x4 F)
    {
        Matrix4x4 C = Matrix4x4.zero;
        for (int ii = 0; ii < 3; ii++)
            for (int jj = 0; jj < 3; jj++)
                for (int kk = 0; kk < 3; kk++)
                    C[ii, jj] += F[kk, ii] * F[kk, jj];

        Matrix4x4 C2 = Matrix4x4.zero;
        for (int ii = 0; ii < 3; ii++)
            for (int jj = 0; jj < 3; jj++)
                for (int kk = 0; kk < 3; kk++)
                    C2[ii, jj] += C[ii, kk] * C[jj, kk];

        float det = F[0, 0] * F[1, 1] * F[2, 2] +
                        F[0, 1] * F[1, 2] * F[2, 0] +
                        F[1, 0] * F[2, 1] * F[0, 2] -
                        F[0, 2] * F[1, 1] * F[2, 0] -
                        F[0, 1] * F[1, 0] * F[2, 2] -
                        F[0, 0] * F[1, 2] * F[2, 1];

        float I_c = C[0, 0] + C[1, 1] + C[2, 2];
        float I_c2 = I_c * I_c;
        float II_c = 0.5f * (I_c2 - C2[0, 0] - C2[1, 1] - C2[2, 2]);
        float III_c = det * det;
        float k = I_c2 - 3 * II_c;

        Matrix4x4 inv_U = Matrix4x4.zero;
        if (k < 1e-10f)
        {
            float inv_lambda = 1 / Mathf.Sqrt(I_c / 3);
            inv_U[0, 0] = inv_lambda;
            inv_U[1, 1] = inv_lambda;
            inv_U[2, 2] = inv_lambda;
        }
        else
        {
            float l = I_c * (I_c * I_c - 4.5f * II_c) + 13.5f * III_c;
            float k_root = Mathf.Sqrt(k);
            float value = l / (k * k_root);
            if (value < -1.0f) value = -1.0f;
            if (value > 1.0f) value = 1.0f;
            float phi = Mathf.Acos(value);
            float lambda2 = (I_c + 2 * k_root * Mathf.Cos(phi / 3)) / 3.0f;
            float lambda = Mathf.Sqrt(lambda2);

            float III_u = Mathf.Sqrt(III_c);
            if (det < 0) III_u = -III_u;
            float I_u = lambda + Mathf.Sqrt(-lambda2 + I_c + 2 * III_u / lambda);
            float II_u = (I_u * I_u - I_c) * 0.5f;

            float inv_rate, factor;
            inv_rate = 1 / (I_u * II_u - III_u);
            factor = I_u * III_u * inv_rate;

            Matrix4x4 U = Matrix4x4.zero;
            U[0, 0] = factor;
            U[1, 1] = factor;
            U[2, 2] = factor;

            factor = (I_u * I_u - II_u) * inv_rate;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    U[i, j] += factor * C[i, j] - inv_rate * C2[i, j];

            inv_rate = 1 / III_u;
            factor = II_u * inv_rate;
            inv_U[0, 0] = factor;
            inv_U[1, 1] = factor;
            inv_U[2, 2] = factor;

            factor = -I_u * inv_rate;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    inv_U[i, j] += factor * U[i, j] + inv_rate * C[i, j];
        }

        Matrix4x4 R = Matrix4x4.zero;
        for (int ii = 0; ii < 3; ii++)
            for (int jj = 0; jj < 3; jj++)
                for (int kk = 0; kk < 3; kk++)
                    R[ii, jj] += F[ii, kk] * inv_U[kk, jj];
        R[3, 3] = 1;
        return R;
    }

    // Update the mesh vertices according to translation c and rotation R.
    // It also updates the velocity.
    private void Update_Mesh(Vector3 c, Matrix4x4 R)
    {
        for (int i = 0; i < Q.Length; i++)
        {
            Vector3 x = (Vector3)(R * Q[i]) + c;

            V[i] += (x - X[i]) * inv_dt;
            X[i] = x;
        }
        Mesh mesh = GetComponent<MeshFilter>().mesh;
        mesh.vertices = X;
    }

    private void CalculateAStep1(int i)
    {
        Vector3 yi_c = X[i] - c;
        A[0, 0] += yi_c[0] * Q[i][0];
        A[0, 1] += yi_c[0] * Q[i][1];
        A[0, 2] += yi_c[0] * Q[i][2];
        A[1, 0] += yi_c[1] * Q[i][0];
        A[1, 1] += yi_c[1] * Q[i][1];
        A[1, 2] += yi_c[1] * Q[i][2];
        A[2, 0] += yi_c[2] * Q[i][0];
        A[2, 1] += yi_c[2] * Q[i][1];
        A[2, 2] += yi_c[2] * Q[i][2];
    }

    // In this function, update v and w by the impulse due to the collision with
    //a plane <P, N>
    private void Collision_Impulse(Vector3 P, Vector3 N)
    {
        for (int i = 0; i < X.Length; i++)
        {
            bool isCollision = Vector3.Dot(X[i] - P, N) < 0;
            if (!isCollision) { continue; }

            bool isV = Vector3.Dot(V[i], N) < 0;
            if (!isV) { continue; }

            Vector3 vn_i = Vector3.Dot(V[i], N) * N;
            Vector3 vt_i = V[i] - vn_i;

            vn_i = vn_i.magnitude < min_velocity ? Vector3.zero : vn_i;
            vt_i = vt_i.magnitude < min_velocity ? Vector3.zero : vt_i;

            float a = Mathf.Max(1 - ut * (1 + un) * vn_i.magnitude / vt_i.magnitude, 0);

            Vector3 vn_new_i = -un * vn_i;
            Vector3 vt_new_i = a * vt_i;

            V[i] = vn_new_i + vt_new_i;
        }
    }

    private void Collision()
    {
        Collision_Impulse(new Vector3(0, 0.01f, 0), new Vector3(0, 1, 0));
        Collision_Impulse(new Vector3(2, 0, 0), new Vector3(-1, 0, 0));
    }

    // Update is called once per frame
    private void Update()
    {
        if (!launched)
        {
            return;
        }

        //Step 1: run a simple particle system.
        UnityEngine.Profiling.Profiler.BeginSample("Particle System");
        c = Vector3.zero;
        for (int i = 0; i < V.Length; i++)
        {
            V[i] = (V[i] + dt * g) * linear_decay;
            //V[i] *= linear_decay;
            X[i] += dt * V[i];
            c += X[i];
        }
        c /= V.Length;
        UnityEngine.Profiling.Profiler.EndSample();

        UnityEngine.Profiling.Profiler.BeginSample("Collision");
        //Step 2: Perform simple particle collision.
        Collision();
        UnityEngine.Profiling.Profiler.EndSample();

        // Step 3: Use shape matching to get new translation c and
        // new rotation R. Update the mesh by c and R.
        UnityEngine.Profiling.Profiler.BeginSample("Get A Step 1");

        A = Matrix4x4.zero;
        Parallel.For(0, V.Length, CalculateAStep1);

        UnityEngine.Profiling.Profiler.EndSample();

        UnityEngine.Profiling.Profiler.BeginSample("Get A Step 2");
        A[3, 3] = 1;
        A *= QQt_i;
        UnityEngine.Profiling.Profiler.EndSample();

        UnityEngine.Profiling.Profiler.BeginSample("Get Rotation");
        Matrix4x4 R = Get_Rotation(A);
        UnityEngine.Profiling.Profiler.EndSample();

        UnityEngine.Profiling.Profiler.BeginSample("Update Mesh");
        Update_Mesh(c, R);
        UnityEngine.Profiling.Profiler.EndSample();
    }

}